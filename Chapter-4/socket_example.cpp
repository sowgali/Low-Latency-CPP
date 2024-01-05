#include "logging.hpp"
#include "time_utils.hpp"
#include "tcp_server.hpp"


int main(int, char**){
    using namespace Common;
    std::string time_str_;
    Logger logger("socket_example.log");
    auto tcpServerRecvCallback = [&](TCPSocket* s, Nanos rx_time) noexcept {
        logger.log("TCPServer::defaultRecvCallback() socket:%, len: %, rx:%\n", std::to_string(s->getFD()), s->getNextRecvValidIdx(), rx_time);
        const std::string reply = "TCPServer received msg:" + std::string(s->getRecvBuff(), s->getNextRecvValidIdx());
        s->setNextRecvValidIdx(0);
        s->send(reply.data(), reply.length());
    };

    auto tcpServerRecvFinishedCallback = [&]() noexcept {
        logger.log("TCPServer::defaultRecvFinishedCallback()\n");
    };

    auto tcpClientRecvCallback = [&](TCPSocket* s, Nanos rx_time) noexcept {
        const std::string recv_msg = std::string(s->getRecvBuff(), s->getNextRecvValidIdx());
        s->setFD(0);
        logger.log("TCPSocket::defaultRecvCallback() socket:% len:% rx:% msg:%\n", s->getFD(), s->getNextRecvValidIdx(), rx_time, recv_msg);
    };

    const std::string iface = "lo";
    const std::string ip = "127.0.0.1";
    const int port = 12345;
    logger.log("Creating server with iface: % and ip: %\n", iface, ip);
    TCPServer tcpServer(logger);
    tcpServer.recv_callback_ = tcpServerRecvCallback;
    tcpServer.recv_finished_callback_ = tcpServerRecvFinishedCallback;
    tcpServer.listen(iface, port);
    std::vector<TCPSocket*> clients(5);
    for(size_t i = 0; i < clients.size(); i++){
        clients[i] = new TCPSocket(logger);
        clients[i]->setRecvCallback(tcpClientRecvCallback);
        logger.log("Connecting TCPClient-[%] on ip:% iface:% port:%\n", i, ip, iface, port);
        clients[i]->connect(ip, iface, port, false);
        tcpServer.poll();
    }

    using namespace std::literals::chrono_literals;
    for(int j = 0; j < 5; j++){
        for(size_t i = 0; i < clients.size(); i++){
            const std::string client_msg = "CLIENT-[" + std::to_string(i) + "] : Sending " + std::to_string(j * 100 + i);
            logger.log("Sending TCPClient-[%] %\n", i, client_msg);
            clients[i]->send(client_msg.data(), client_msg.length());
            clients[i]->sendAndRecv();
            std::this_thread::sleep_for(500ms);
            tcpServer.poll();
            tcpServer.sendAndRecv();
        }
    }
}