#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMqttClient>

#include "machinehandle.h"
#include "machine.pb.h"
#include <any>
#include <string>


#include <unordered_map>
#include <functional>

typedef std::unordered_map<std::string, std::function<std::string()>> func_map;
typedef std::unordered_map<std::string, std::function<QString(QByteArray const&)>> pb_map;

namespace Ui {
class MainWindow;
}

struct session {
    int org_id;
    std::string work_sight;
    int session;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr, std::string xml_config = "templatetest.xml");
    void publish_message();
    ~MainWindow();

public slots:
    void setClientPort(int p);

private slots:
    void on_buttonConnect_clicked();
    void on_buttonQuit_clicked();
    void updateLogStateChange();

    void brokerDisconnected();

    void on_buttonPublish_clicked();
    void on_publish_machine_button_clicked();

    void on_buttonSubscribe_clicked();
    void on_subscribe_machine_button_clicked();

private:
    Ui::MainWindow *ui;
    QMqttClient *m_client;
    MachineHandle machine;
    session current_session;
    pb_map parse_from_topic_to_protobuf;
    func_map generate_proto_from_topic;
    void subscribe_to_messages(const std::vector<std::string>&);
};

#endif // MAINWINDOW_H
