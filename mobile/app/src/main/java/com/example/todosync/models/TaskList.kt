package com.example.todosync.models

data class TaskList(
    var name: String,
    var tasks: MutableList<Task> = mutableListOf()
)