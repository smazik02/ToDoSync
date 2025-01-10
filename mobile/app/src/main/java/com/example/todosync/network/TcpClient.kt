package com.example.todosync.network

import android.util.Log
import kotlinx.coroutines.CoroutineScope
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.SupervisorJob
import kotlinx.coroutines.launch
import java.io.BufferedReader
import java.io.IOException
import java.io.OutputStream
import java.io.InputStream
import java.io.InputStreamReader
import java.net.Socket

object TcpClientSingleton {
    val tcpClient: TcpClient by lazy {
        TcpClient("10.0.2.2", 12345)
    }
}

class TcpClient(private val serverIp: String, private val serverPort: Int) {

    private var socket: Socket? = null
    private var outputStream: OutputStream? = null
    private var inputStream: InputStream? = null

    private val scope = CoroutineScope(Dispatchers.IO + SupervisorJob())

    var onMessageReceived: ((String) -> Unit)? = null

    fun connect() {
        scope.launch {
            try {
                socket = Socket(serverIp, serverPort)
                outputStream = socket?.getOutputStream()
                inputStream = socket?.getInputStream()

                listenForMessages()
            } catch (e: IOException) {
                e.printStackTrace()
            }
        }
    }

    fun sendMessage(message: String) {
        scope.launch {
            try {
                outputStream?.write(message.toByteArray())
                outputStream?.flush()
            } catch (e: IOException) {
                e.printStackTrace()
            }
        }
    }

    fun listenForMessages() {
        try {
            val reader = BufferedReader(InputStreamReader(inputStream))
            val buffer = StringBuilder()

            while (true) {
                val line = reader.readLine() ?: break

                buffer.append(line).append("\n")

                if (buffer.endsWith("\n\n")) {
                    val fullMessage = buffer.toString().trimEnd('\n')

                    onMessageReceived?.invoke(fullMessage)
                    buffer.clear()
                }
            }
        } catch (e: IOException) {
            e.printStackTrace()
        } finally {
            disconnect()
        }
    }

    fun disconnect() {
        try {
            socket?.close()
        } catch (e: IOException) {
            e.printStackTrace()
        }
    }
}