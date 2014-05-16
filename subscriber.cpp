#include <QDebug>
#include <QThread>

#include <msgpack.hpp>

#include "subscriber.h"

Subscriber::Subscriber(QObject *parent) :
    QObject(parent)
{
    context = new zmq::context_t(1);
    socket = new zmq::socket_t(*context, ZMQ_SUB);
    socket->setsockopt(ZMQ_SUBSCRIBE, "", 0);
}

Subscriber::~Subscriber()
{
    socket->close();
    emit disconnected();
}

void Subscriber::startListening()
{
    try
    {
        socket->connect("tcp://127.0.0.1:5555");
    }
    catch(zmq::error_t err)
    {
        QString str = QString::fromUtf8(err.what());
        qDebug() << str ;
        emit  errored(str);
    }

    emit connected();

    while(!QThread::currentThread()->isInterruptionRequested())
    {
        zmq::message_t message;
        if(socket->recv(&message, ZMQ_NOBLOCK))
        {
            msgpack::unpacked unpackedMessage;
            msgpack::unpack(&unpackedMessage, (const char *)message.data(), message.size());
            msgpack::object_raw rawMessage = unpackedMessage.get().via.raw;
            QString strMessage = QString::fromUtf8(rawMessage.ptr, rawMessage.size);
            //qDebug() << strMessage;
            emit messageReady(strMessage);
        }
    }
}
