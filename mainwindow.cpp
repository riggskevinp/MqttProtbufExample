#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QtCore/QDateTime>
#include <QtMqtt/QMqttClient>
#include <QtWidgets/QMessageBox>

#include <iostream>
#include <fstream>
#include <cstdlib>

template<typename ProtoDefinition>
QString proto_to_string(const QByteArray & msg){
    ProtoDefinition proto;
    if(proto.ParseFromArray(msg, msg.size())){
        return QString::fromStdString(proto.ShortDebugString());
    } else {
        std::cout << "Proto parse from array failed" << std::endl;
        return QString("");
    }

}

MainWindow::MainWindow(QWidget *parent, std::string xml_config) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    machine(xml_config),
    current_session{123456, "R_D_JobSite", 21}
{
    ui->setupUi(this);

    parse_from_topic_to_protobuf = {
        {"GnssLocation", proto_to_string<GnssLocation>},
        {"TruckState", proto_to_string<TruckState>},
        {"Model", proto_to_string<Model>},
        {"Diagnostic", proto_to_string<Diagnostic>},
        {"ArmAngles", proto_to_string<ArmAngles>},
    };

    generate_proto_from_topic = {
        {"GnssLocation", [](){
             GnssLocation x;
             x.set_latitude(std::rand() % 100);
             x.set_longitude(std::rand() % 100);
             return x.SerializeAsString();
         }},
        {"TruckState", [](){
             TruckState x;
             x.set_current_state(TruckState::MachineState(std::rand() % 4));
             return x.SerializeAsString();
         }},
        {"Model", [](){
             Model x;
             x.set_model_name("Temp");
             x.set_serial_number(std::rand() % 100);
             return x.SerializeAsString();
         }},
        {"Diagnostic", [](){
             Diagnostic x;
             auto s = std::rand() % 100;
             std::cout << "diag int " << s << std::endl;
             x.set_engine_speed(s);
             return x.SerializeAsString();
         }},
        {"ArmAngles", [](){
             ArmAngles x;
             x.set_arm_angle(std::rand() % 100);
             x.set_boom_angle(std::rand() % 100);
             x.set_bucket_angle(std::rand() % 100);
             x.set_house_to_chassis(std::rand() % 100);
             return x.SerializeAsString();
         }},
    };

    m_client = new QMqttClient(this);
    m_client->setHostname(ui->lineEditHost->text());
    m_client->setPort(ui->spinBoxPort->value());
    current_session.session = ui->spinBoxSession->value();
    current_session.org_id = ui->spinBoxOrg->value();
    current_session.work_sight = ui->lineEditWorksite->text().toStdString();

    connect(m_client, &QMqttClient::stateChanged, this, &MainWindow::updateLogStateChange);
    connect(m_client, &QMqttClient::disconnected, this, &MainWindow::brokerDisconnected);

    connect(m_client, &QMqttClient::messageReceived, this, [this](const QByteArray &message, const QMqttTopicName &topic) {
        QString msg;
        std::string top_level_topic = topic.levels().last().toStdString();
        if(parse_from_topic_to_protobuf.find(top_level_topic) != parse_from_topic_to_protobuf.end()){
            msg = parse_from_topic_to_protobuf.at(top_level_topic)(message);
        } else {
            msg = QString(message);
        }
        const QString content = QDateTime::currentDateTime().toString()
                    + QLatin1String(" Received Topic: ")
                    + topic.name()
                    + QLatin1String(" Message: ")
                    + msg
                    + QLatin1Char('\n');
        ui->editLog->insertPlainText(content);
    });

    connect(m_client, &QMqttClient::pingResponseReceived, this, [this]() {
        const QString content = QDateTime::currentDateTime().toString()
                    + QLatin1String(" PingResponse")
                    + QLatin1Char('\n');
        ui->editLog->insertPlainText(content);
    });


    connect(ui->lineEditHost, &QLineEdit::textChanged, m_client, &QMqttClient::setHostname);
    connect(ui->spinBoxPort, QOverload<int>::of(&QSpinBox::valueChanged), this, &MainWindow::setClientPort);
    connect(ui->lineEditWorksite, &QLineEdit::textChanged, this, [&](const QString &work_site_name){this->current_session.work_sight = work_site_name.toStdString();});
    connect(ui->spinBoxOrg, QOverload<int>::of(&QSpinBox::valueChanged), this, [&](int org){this->current_session.org_id = org;});
    connect(ui->spinBoxSession, QOverload<int>::of(&QSpinBox::valueChanged), this, [&](int session){this->current_session.session = session;});
    updateLogStateChange();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_buttonConnect_clicked()
{
    if (m_client->state() == QMqttClient::Disconnected) {
        ui->lineEditHost->setEnabled(false);
        ui->spinBoxPort->setEnabled(false);
        ui->buttonConnect->setText(tr("Disconnect"));
        m_client->connectToHost();
    } else {
        ui->lineEditHost->setEnabled(true);
        ui->spinBoxPort->setEnabled(true);
        ui->buttonConnect->setText(tr("Connect"));
        m_client->disconnectFromHost();
    }
}

void MainWindow::on_buttonQuit_clicked()
{
    QApplication::quit();
}

void MainWindow::updateLogStateChange()
{
    const QString content = QDateTime::currentDateTime().toString()
                    + QLatin1String(": State Change")
                    + QString::number(m_client->state())
                    + QLatin1Char('\n');
    ui->editLog->insertPlainText(content);
}

void MainWindow::brokerDisconnected()
{
    ui->lineEditHost->setEnabled(true);
    ui->spinBoxPort->setEnabled(true);
    ui->buttonConnect->setText(tr("Connect"));
}

void MainWindow::setClientPort(int p)
{
    m_client->setPort(p);
}

void MainWindow::on_buttonPublish_clicked()
{
    QStringList topic_list = ui->lineEditTopic->text().split("/");
    std::string topic = topic_list.last().toStdString();
    if(generate_proto_from_topic.find(topic) != generate_proto_from_topic.end()){
        std::string payload = generate_proto_from_topic.at(topic)();
        auto serialized_payload = QByteArray(payload.c_str(), payload.size());
        if (m_client->publish(ui->lineEditTopic->text(), serialized_payload) == -1){
            QMessageBox::critical(this, QLatin1String("Error"), QLatin1String("Could not publish message"));
        }
    } else {
        if (m_client->publish(ui->lineEditTopic->text(), ui->lineEditMessage->text().toUtf8()) == -1){
            QMessageBox::critical(this, QLatin1String("Error"), QLatin1String("Could not publish message"));
        }
    }
}

void MainWindow::on_publish_machine_button_clicked(){
    for(const auto topic : machine.publish_messages){
        std::string topic_prefix = "/" + std::to_string(current_session.org_id) + "/" + current_session.work_sight + "/" + std::to_string(current_session.session) + "/" + machine.model + std::to_string(machine.serial_number) + "/";
        if(generate_proto_from_topic.find(topic) != generate_proto_from_topic.end()){
            std::string payload = generate_proto_from_topic.at(topic)();
            auto serialized_payload = QByteArray(payload.c_str(), payload.size());
            if (m_client->publish(QString::fromStdString(topic_prefix + topic), serialized_payload) == -1){
                QMessageBox::critical(this, QLatin1String("Error"), QLatin1String("Could not publish message"));
            }
        }
    }
}

void MainWindow::on_buttonSubscribe_clicked()
{
    auto subscription = m_client->subscribe(ui->lineEditTopic->text());
    if (!subscription) {
        QMessageBox::critical(this, QLatin1String("Error"), QLatin1String("Could not subscribe. Is there a valid connection?"));
        return;
    } /*else {
        connect(subscription, &QMqttSubscription::messageReceived, this, [&](QMqttMessage msg){msg.payload();});
    }*/
}

void MainWindow::subscribe_to_messages(const std::vector<std::string> & topics)
{
    std::string prefix = "/" + std::to_string(current_session.org_id) + "/" + current_session.work_sight + "/" + std::to_string(current_session.session) + "/+/";

    for(const auto topic : topics){
        auto topic_str = prefix + topic;
        auto subscription = m_client->subscribe(QString::fromStdString(topic_str));
        if (!subscription) {
            QMessageBox::critical(this, QLatin1String("Error"), QLatin1String("Could not subscribe. Is there a valid connection?"));
            return;
        }
    }
}

void MainWindow::on_subscribe_machine_button_clicked()
{
    subscribe_to_messages(machine.subscription_messages);
}
