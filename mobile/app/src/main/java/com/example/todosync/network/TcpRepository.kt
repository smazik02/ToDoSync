package com.example.todosync.network

import android.util.Log
import kotlinx.coroutines.CoroutineScope
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.flow.MutableSharedFlow
import kotlinx.coroutines.flow.asSharedFlow
import kotlinx.coroutines.launch
import org.json.JSONException
import org.json.JSONObject
import java.io.IOException

enum class ConnectionState { CONNECTED, DISCONNECTED }

enum class MessageType { OK, FAIL, NOTIFY, DISCONNECT }

data class ReceivedMessage(val type: MessageType, val body: JSONObject)

class TcpRepository {

    private val tcpClient = TcpClientSingleton.tcpClient

    private val _serverMessages = MutableSharedFlow<ReceivedMessage>()
    val serverMessages = _serverMessages.asSharedFlow()

    private var activeUser: String? = null

    init {
        tcpClient.onMessageReceived = { message ->
            CoroutineScope(Dispatchers.IO).launch {
                val parsedMessage = parseMessage(message)
                parsedMessage?.let { _serverMessages.emit(it) }
            }
        }

        CoroutineScope(Dispatchers.IO).launch {
            val disconnect = tcpClient.disconnectChannel.receive()

            if (disconnect == ConnectionState.DISCONNECTED) {
                _serverMessages.emit(ReceivedMessage(MessageType.DISCONNECT, JSONObject()))
            }
        }
    }

    private fun connect(address: String) {
        if (!address.contains(":"))
            throw IOException("Cannot parse address")

        val (serverIp, serverPort) = address.split(":")
        tcpClient.serverIp = serverIp
        tcpClient.serverPort = serverPort.toInt()
        Log.d("TcpRepositoryConnect", "Trying to connect")
        tcpClient.connect()
        Log.d("TcpRepositoryConnect", "Connected")
    }

    private fun sendMessage(message: String) {
        tcpClient.sendMessage(message)
    }

    fun disconnect() {
        tcpClient.disconnect()
    }

    private fun parseMessage(message: String): ReceivedMessage? {
        Log.d("ParseMessage", message)
        val (header, body) = message.split("\n")
        if (header == "" || body == "") {
            return null
        }

        val messageType = when (header) {
            "OK" -> MessageType.OK
            "FAIL" -> MessageType.FAIL
            "NOTIFY" -> MessageType.NOTIFY
            else -> return null
        }

        try {
            val messageBody = JSONObject(body)
            return ReceivedMessage(messageType, messageBody)
        } catch (e: JSONException) {
            return null
        }
    }

    // -- Specialized methods --
    suspend fun login(address: String, login: String) {
        Log.d("TcpRepositoryLogin", "Trying to connect")
        connect(address)

        Log.d("TcpRepositoryLogin", "Before receive")
        val connectionState = tcpClient.statusChannel.receive()
        if (connectionState == ConnectionState.CONNECTED) {
            Log.d("TcpRepositoryLogin", "State connected")
            val body = JSONObject()
            body.put("username", login)

            sendMessage(createRequest(TcpClientMethod.AUTH_LOGIN, body))
            activeUser = login
        } else {
            Log.d("TcpRepositoryLogin", "State disconnected")
            throw IOException("Connection error")
        }
    }

    fun taskGetAll(taskListName: String) {
        val body = JSONObject()
        body.put("task_list_name", taskListName)
        sendMessage(createRequest(TcpClientMethod.T_GET_ALL, body))
    }

    fun taskCreate(
        taskListName: String,
        taskName: String,
        taskDescription: String
    ) {
        val body = JSONObject()
        body.put("task_list_name", taskListName)
        body.put("task_name", taskName)
        body.put("task_description", taskDescription)
        sendMessage(createRequest(TcpClientMethod.T_CREATE, body))
    }

    fun taskDelete(taskId: Int, taskListName: String) {
        val body = JSONObject()
        body.put("task_list_name", taskListName)
        body.put("task_id", taskId)
        sendMessage(createRequest(TcpClientMethod.T_DELETE, body))
    }

    fun taskListGetAll() {
        sendMessage(createRequest(TcpClientMethod.TL_GET_ALL))
    }

    fun taskListCreate(name: String) {
        val body = JSONObject()
        body.put("name", name)
        sendMessage(createRequest(TcpClientMethod.TL_CREATE, body))
    }

    fun taskListJoin(name: String) {
        val body = JSONObject()
        body.put("name", name)
        sendMessage(createRequest(TcpClientMethod.TL_JOIN, body))
    }

    private fun createRequest(
        method: TcpClientMethod,
        body: JSONObject = JSONObject()
    ): String {
        return method.v + "\n" + body.toString() + "\n\n"
    }
}