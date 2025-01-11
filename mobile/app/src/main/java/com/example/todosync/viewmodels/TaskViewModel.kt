package com.example.todosync.viewmodels

import androidx.lifecycle.ViewModel
import com.example.todosync.models.Task
import com.example.todosync.network.TcpRepository
import kotlinx.coroutines.flow.MutableStateFlow
import kotlinx.coroutines.flow.asStateFlow
import kotlinx.coroutines.flow.update

data class TaskUiState(
    val taskListName: String = "",
    val taskList: MutableList<Task> = mutableListOf()
)

class TaskViewModel(private val tcpRepository: TcpRepository) : ViewModel() {

    private val _uiState = MutableStateFlow(TaskUiState())
    val uiState = _uiState.asStateFlow()

    fun loadTasks(taskListName: String) {
        _uiState.update { currentState ->
            currentState.copy(
                taskList = mutableListOf(
                    Task(1, "Task1", "Description"),
                    Task(2, "Task2", "Helloooo")
                )
            )
        }
    }

    fun addTask(newTask: Task) {
        _uiState.update { currentState ->
            currentState.copy(
                taskList = mutableListOf(*(currentState.taskList.toTypedArray()), newTask)
            )
        }
    }

    fun removeTask(taskId: Int?) {
        if (taskId == null) return
        _uiState.update { currentState ->
            currentState.copy(
                taskList = currentState.taskList.filter { it.id != taskId }.toMutableList()
            )
        }
    }

}