package com.example.todosync.network

import android.util.Log
import kotlinx.coroutines.CoroutineScope
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.flow.MutableSharedFlow
import kotlinx.coroutines.flow.MutableStateFlow
import kotlinx.coroutines.flow.asSharedFlow
import kotlinx.coroutines.flow.asStateFlow
import kotlinx.coroutines.launch
import org.json.JSONObject
import java.io.IOException
import java.net.UnknownHostException

enum class ConnectionState(val v: String) {
    CONNECTED("CONNECTED"),
    DISCONNECTED("DISCONNECTED")
}

class TcpRepository {

    private val tcpClient = TcpClientSingleton.tcpClient

    private val _serverMessages = MutableSharedFlow<String>()
    val serverMessages = _serverMessages.asSharedFlow()

    private val _connectionState = MutableStateFlow(ConnectionState.DISCONNECTED)
    val connectionState = _connectionState.asStateFlow()

    private var activeUser: String? = null

    init {
        tcpClient.onMessageReceived = { message ->
            CoroutineScope(Dispatchers.IO).launch {
                _serverMessages.emit(message)
            }
        }
    }

    fun connect(address: String) {
        if (!address.contains(":"))
            throw IOException("Cannot parse address")

        val (serverIp, serverPort) = address.split(":")
        tcpClient.serverIp = serverIp
        tcpClient.serverPort = serverPort.toInt()
        tcpClient.connect()
    }

    fun sendMessage(message: String) {
        tcpClient.sendMessage(message)
    }

    fun disconnect() {
        tcpClient.disconnect()
        _connectionState.value = ConnectionState.DISCONNECTED
    }

    // -- Specialized methods --
    suspend fun login(address: String, login: String) {
        try {
            connect(address)

            _connectionState.value = tcpClient.statusChannel.receive()
            if ( _connectionState.value == ConnectionState.CONNECTED) {
                val body = JSONObject()
                body.put("username", login)

                sendMessage(createRequest(TcpClientMethod.AUTH_LOGIN, body))
                activeUser = login
            }
        } catch (e: IOException) {
            e.printStackTrace()
            Log.d("CONNECT", e.message.toString())
            Log.d("CONNECT", "Connection fail")
        } catch (e: UnknownHostException) {
            e.printStackTrace()
        }
    }

    suspend fun taskGetAll() {
        sendMessage(createRequest(TcpClientMethod.T_GET_ALL))
    }

    suspend fun taskCreate(
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

    suspend fun taskDelete(taskId: String, taskListName: String) {
        val body = JSONObject()
        body.put("task_list_name", taskListName)
        body.put("task_id", taskId)
        sendMessage(createRequest(TcpClientMethod.T_DELETE, body))
    }

    suspend fun taskListGetAll() {
        sendMessage(createRequest(TcpClientMethod.TL_GET_ALL))
    }

    suspend fun taskListCreate(name: String) {
        val body = JSONObject()
        body.put("name", name)
        sendMessage(createRequest(TcpClientMethod.TL_CREATE, body))
    }

    suspend fun taskListJoin(name: String) {
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