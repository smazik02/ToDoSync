package com.example.todosync.viewmodels

import android.util.Log
import androidx.lifecycle.ViewModel
import androidx.lifecycle.ViewModelProvider
import androidx.lifecycle.ViewModelProvider.AndroidViewModelFactory.Companion.APPLICATION_KEY
import androidx.lifecycle.viewModelScope
import androidx.lifecycle.viewmodel.CreationExtras
import com.example.todosync.ToDoSyncApplication
import com.example.todosync.models.Task
import com.example.todosync.network.MessageType
import com.example.todosync.network.ReceivedMessage
import com.example.todosync.network.TcpRepository
import kotlinx.coroutines.flow.MutableStateFlow
import kotlinx.coroutines.flow.asStateFlow
import kotlinx.coroutines.flow.update
import kotlinx.coroutines.launch

data class TaskUiState(
    val taskListName: String = "",
    val taskList: MutableList<Task> = mutableListOf()
)

class TaskViewModel(private val tcpRepository: TcpRepository) : ViewModel() {

    private val _uiState = MutableStateFlow(TaskUiState())
    val uiState = _uiState.asStateFlow()

    init {
        viewModelScope.launch {
            tcpRepository.serverMessages.collect { message ->
                handleIncomingMessage(message)
            }
        }
    }

    private fun handleIncomingMessage(message: ReceivedMessage) {
        when (message.type) {
            MessageType.OK -> {
                if (message.body.has("tasks")) {
                    val tasksArray = message.body.getJSONArray("tasks")

                    val tasks = mutableListOf<Task>()
                    for (i in 0..tasksArray.length()-1) {
                        val taskObj = tasksArray.getJSONObject(i)
                        tasks.add(Task(
                            taskObj.getInt("id"),
                            taskObj.getString("title"),
                            taskObj.getString("description")
                        ))
                    }

                    _uiState.update { currentState ->
                        currentState.copy(
                            taskList = tasks
                        )
                    }
                } else if (message.body.has("source") && message.body.getString("source") == "T") {
                    viewModelScope.launch {
                        tcpRepository.taskGetAll(uiState.value.taskListName)
                    }
                }
            }

            MessageType.FAIL -> {
                if (message.body.has("source") && message.body.getString("source") == "T") {
//                    _event.value = message.body.getString("message")
                }
            }
            MessageType.NOTIFY -> {}
        }
    }

    fun loadTasks(taskListName: String) {
        viewModelScope.launch {
            _uiState.update { currentState ->
                currentState.copy(
                    taskListName
                )
            }

            tcpRepository.taskGetAll(taskListName)
        }
    }

    fun addTask(newTask: Task) {
        viewModelScope.launch {
            tcpRepository.taskCreate(
                uiState.value.taskListName,
                newTask.title,
                newTask.description
            )
        }
    }

    fun removeTask(taskId: Int?) {
        if (taskId == null) return

        viewModelScope.launch {
            tcpRepository.taskDelete(taskId, uiState.value.taskListName)
        }
    }


    companion object {
        val Factory: ViewModelProvider.Factory = object : ViewModelProvider.Factory {
            override fun <T : ViewModel> create(modelClass: Class<T>, extras: CreationExtras): T {
                val application = checkNotNull(extras[APPLICATION_KEY])
                return TaskViewModel((application as ToDoSyncApplication).tcpRepository) as T
            }
        }
    }
}