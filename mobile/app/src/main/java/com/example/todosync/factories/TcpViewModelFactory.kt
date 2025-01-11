package com.example.todosync.factories

import androidx.lifecycle.ViewModel
import androidx.lifecycle.ViewModelProvider
import com.example.todosync.network.TcpRepository
import com.example.todosync.viewmodels.TaskListViewModel
import com.example.todosync.viewmodels.TaskViewModel

class TcpViewModelFactory(private val repository: TcpRepository): ViewModelProvider.Factory {
    override fun <T: ViewModel> create(modelClass: Class<T>): T {
        if (modelClass.isAssignableFrom(TaskListViewModel::class.java)) {
            return TaskListViewModel(repository) as T
        }
        if (modelClass.isAssignableFrom(TaskViewModel::class.java)) {
            return TaskViewModel(repository) as T
        }
        throw IllegalArgumentException("Unknown view model class")
    }
}
