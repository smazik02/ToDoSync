package com.example.todosync

import android.app.Application
import com.example.todosync.network.TcpRepository

class ToDoSyncApplication : Application() {
    val tcpRepository: TcpRepository by lazy {
        TcpRepository()
    }
}