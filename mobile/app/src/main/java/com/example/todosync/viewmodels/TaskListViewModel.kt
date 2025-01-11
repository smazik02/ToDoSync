package com.example.todosync.viewmodels

import androidx.lifecycle.LiveData
import androidx.lifecycle.MutableLiveData
import androidx.lifecycle.ViewModel
import androidx.lifecycle.ViewModelProvider
import androidx.lifecycle.ViewModelProvider.AndroidViewModelFactory.Companion.APPLICATION_KEY
import androidx.lifecycle.viewModelScope
import androidx.lifecycle.viewmodel.CreationExtras
import com.example.todosync.ToDoSyncApplication
import com.example.todosync.models.TaskList
import com.example.todosync.network.MessageType
import com.example.todosync.network.ReceivedMessage
import com.example.todosync.network.TcpRepository
import kotlinx.coroutines.flow.MutableStateFlow
import kotlinx.coroutines.flow.asStateFlow
import kotlinx.coroutines.flow.update
import kotlinx.coroutines.launch

data class TaskListUiState(
    val taskListList: MutableList<TaskList> = mutableListOf()
)

class TaskListViewModel(private val tcpRepository: TcpRepository) : ViewModel() {

    private val _uiState = MutableStateFlow(TaskListUiState())
    val uiState = _uiState.asStateFlow()

    private val _event = MutableLiveData<String>()
    val event: LiveData<String> get() = _event

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
                if (message.body.has("lists")) {
                    val taskListArray = message.body.getJSONArray("lists")
                    val taskLists = mutableListOf<TaskList>()
                    for (i in 0..<taskListArray.length()) {
                        taskLists.add(TaskList(taskListArray.getString(i)))
                    }

                    _uiState.update { TaskListUiState(taskLists) }
                } else if (message.body.has("source") && message.body.getString("source") == "TL") {
                    viewModelScope.launch { tcpRepository.taskListGetAll() }
                    _event.value = message.body.getString("message")
                }
            }

            MessageType.FAIL -> {
                if (message.body.has("source") && message.body.getString("source") == "TL") {
                    _event.value = message.body.getString("message")
                }
            }

            MessageType.NOTIFY -> {}
        }
    }

    fun loadTaskLists() {
        viewModelScope.launch { tcpRepository.taskListGetAll() }
    }

    fun addTaskList(newTaskList: TaskList) {
        viewModelScope.launch { tcpRepository.taskListCreate(newTaskList.name) }
    }

    fun joinTaskList(taskListName: String) {
        viewModelScope.launch { tcpRepository.taskListJoin(taskListName) }
    }

    fun disconnect() {
        viewModelScope.launch { tcpRepository.disconnect() }
    }

    companion object {
        val Factory: ViewModelProvider.Factory = object : ViewModelProvider.Factory {
            @Suppress("UNCHECKED_CAST")
            override fun <T : ViewModel> create(modelClass: Class<T>, extras: CreationExtras): T {
                val application = checkNotNull(extras[APPLICATION_KEY])
                return TaskListViewModel((application as ToDoSyncApplication).tcpRepository) as T
            }
        }
    }

}