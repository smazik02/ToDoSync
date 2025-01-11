package com.example.todosync.viewmodels

import androidx.lifecycle.ViewModel
import com.example.todosync.models.TaskList
import com.example.todosync.network.TcpRepository
import kotlinx.coroutines.flow.MutableStateFlow
import kotlinx.coroutines.flow.asStateFlow
import kotlinx.coroutines.flow.update

data class TaskListUiState(
    val taskListList: MutableList<TaskList> = mutableListOf()
)

class TaskListViewModel(private val tcpRepository: TcpRepository) : ViewModel() {

    private val _uiState = MutableStateFlow(TaskListUiState())
    val uiState = _uiState.asStateFlow()

    fun addTaskList(newTaskList: TaskList) {
        _uiState.update { currentState ->
            currentState.copy(
                taskListList = mutableListOf(
                    *(currentState.taskListList.toTypedArray()),
                    newTaskList
                )
            )
        }
    }

    fun joinTaskList(taskListName: String) {}

}