package com.example.todosync.activities

import android.app.Activity
import android.os.Build
import android.os.Bundle
import android.widget.Toast
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import androidx.activity.enableEdgeToEdge
import androidx.activity.viewModels
import androidx.compose.foundation.BorderStroke
import androidx.compose.foundation.background
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.size
import androidx.compose.foundation.lazy.LazyColumn
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.automirrored.filled.ArrowBack
import androidx.compose.material.icons.filled.Add
import androidx.compose.material.icons.outlined.Delete
import androidx.compose.material3.BottomAppBar
import androidx.compose.material3.Button
import androidx.compose.material3.CardDefaults
import androidx.compose.material3.ExperimentalMaterial3Api
import androidx.compose.material3.FloatingActionButton
import androidx.compose.material3.FloatingActionButtonDefaults
import androidx.compose.material3.Icon
import androidx.compose.material3.IconButton
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.ModalBottomSheet
import androidx.compose.material3.OutlinedCard
import androidx.compose.material3.OutlinedTextField
import androidx.compose.material3.Scaffold
import androidx.compose.material3.Text
import androidx.compose.material3.TopAppBar
import androidx.compose.material3.TopAppBarDefaults
import androidx.compose.material3.rememberModalBottomSheetState
import androidx.compose.material3.rememberTopAppBarState
import androidx.compose.runtime.Composable
import androidx.compose.runtime.collectAsState
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.rememberCoroutineScope
import androidx.compose.runtime.saveable.rememberSaveable
import androidx.compose.runtime.setValue
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.draw.clip
import androidx.compose.ui.input.nestedscroll.nestedScroll
import androidx.compose.ui.platform.LocalContext
import androidx.compose.ui.unit.dp
import androidx.lifecycle.viewmodel.compose.viewModel
import com.example.todosync.models.Task
import com.example.todosync.ui.theme.ToDoSyncTheme
import com.example.todosync.viewmodels.TaskViewModel
import kotlinx.coroutines.launch

class TasksActivity : ComponentActivity() {

    private val viewModel: TaskViewModel by viewModels{ TaskViewModel.Factory }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        enableEdgeToEdge()
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.TIRAMISU) {
            viewModel.loadTasks(
                intent.getSerializableExtra(
                    "EXTRA_TLNAME",
                    String::class.java
                ) as String
            )
        } else {
            @Suppress("DEPRECATION")
            viewModel.loadTasks(intent.getSerializableExtra("EXTRA_TLNAME") as String)
        }
        setContent {
            ToDoSyncTheme {
                Container()
            }
        }
    }

    override fun onResume() {
        super.onResume()
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.TIRAMISU) {
            viewModel.loadTasks(
                intent.getSerializableExtra(
                    "EXTRA_TLNAME",
                    String::class.java
                ) as String
            )
        } else {
            @Suppress("DEPRECATION")
            viewModel.loadTasks(intent.getSerializableExtra("EXTRA_TLNAME") as String)
        }
    }

}

@Composable
fun TaskCard(task: Task) {
    val viewModel = viewModel<TaskViewModel>()

    OutlinedCard(
        modifier = Modifier
            .padding(horizontal = 16.dp, vertical = 8.dp)
            .fillMaxWidth(),
        shape = RoundedCornerShape(12.dp),
        colors = CardDefaults.cardColors(
            containerColor = MaterialTheme.colorScheme.surface,
        ),
        border = BorderStroke(1.dp, MaterialTheme.colorScheme.outlineVariant),
    ) {
        Column {
            Box(
                modifier = Modifier
                    .fillMaxWidth()
                    .clip(RoundedCornerShape(12.dp))
                    .background(MaterialTheme.colorScheme.primary)
                    .padding(1.dp),
            ) {
                Row(
                    modifier = Modifier.fillMaxWidth(),
                    horizontalArrangement = Arrangement.SpaceBetween,
                    verticalAlignment = Alignment.CenterVertically,
                ) {
                    Text(
                        text = task.title,
                        modifier = Modifier.padding(10.dp, 0.dp),
                        color = MaterialTheme.colorScheme.onPrimary,
                        style = MaterialTheme.typography.titleMedium,
                    )
                    IconButton(onClick = { viewModel.removeTask(task.id) }) {
                        Icon(
                            imageVector = Icons.Outlined.Delete,
                            tint = MaterialTheme.colorScheme.onPrimary,
                            contentDescription = "Delete task",
                        )
                    }
                }
            }
            Text(
                text = task.description,
                modifier = Modifier.padding(13.dp),
                color = MaterialTheme.colorScheme.onSurface,
                style = MaterialTheme.typography.bodyMedium,
            )
        }
    }
}

@OptIn(ExperimentalMaterial3Api::class)
@Composable
fun Container() {
    val scrollBehavior = TopAppBarDefaults.pinnedScrollBehavior(rememberTopAppBarState())

    val sheetState = rememberModalBottomSheetState()
    val scope = rememberCoroutineScope()
    var isSheetOpen by rememberSaveable { mutableStateOf(false) }

    val activity = LocalContext.current as? Activity

    Scaffold(
        modifier = Modifier
            .fillMaxSize()
            .nestedScroll(scrollBehavior.nestedScrollConnection),
        topBar = {
            TopAppBar(
                title = {
                    Text(
                        text = "Lists",
                        style = MaterialTheme.typography.titleLarge,
                        color = MaterialTheme.colorScheme.onSurface
                    )
                },
                navigationIcon = {
                    IconButton(onClick = { activity?.finish() }) {
                        Icon(
                            imageVector = Icons.AutoMirrored.Filled.ArrowBack,
                            contentDescription = "Back to lists",
                            modifier = Modifier.size(24.dp),
                            tint = MaterialTheme.colorScheme.onSurface
                        )
                    }
                },
                colors = TopAppBarDefaults.topAppBarColors(
                    containerColor = MaterialTheme.colorScheme.surface,
                    titleContentColor = MaterialTheme.colorScheme.primary,
                    scrolledContainerColor = MaterialTheme.colorScheme.surfaceContainer
                ),
                scrollBehavior = scrollBehavior,
            )
        },
        bottomBar = {
            BottomAppBar(
                actions = {},
                containerColor = MaterialTheme.colorScheme.surfaceContainer,
                floatingActionButton = {
                    FloatingActionButton(
                        onClick = {
                            isSheetOpen = true
                        },
                        containerColor = MaterialTheme.colorScheme.primaryContainer,
                        elevation = FloatingActionButtonDefaults.bottomAppBarFabElevation()
                    ) {
                        Icon(
                            Icons.Filled.Add,
                            contentDescription = "Localized description",
                            tint = MaterialTheme.colorScheme.onPrimaryContainer
                        )
                    }
                },
            )
        }
    ) { innerPadding ->
        val viewModel = viewModel<TaskViewModel>()
        val uiState by viewModel.uiState.collectAsState()

        LazyColumn(modifier = Modifier.padding(innerPadding)) {
            items(uiState.taskList.size) { item ->
                TaskCard(uiState.taskList[item])
            }
        }

        if (isSheetOpen) {
            ModalBottomSheet(
                onDismissRequest = { isSheetOpen = false },
                sheetState = sheetState,
                containerColor = MaterialTheme.colorScheme.surfaceContainerLow,
            ) {
                var taskTitleText by remember { mutableStateOf("") }
                var taskDescriptionText by remember { mutableStateOf("") }

                Column(
                    horizontalAlignment = Alignment.CenterHorizontally,
                    verticalArrangement = Arrangement.Center,
                    modifier = Modifier.fillMaxWidth()
                ) {
                    OutlinedTextField(
                        onValueChange = { taskTitleText = it },
                        value = taskTitleText,
                        label = {
                            Text(
                                text = "Title",
                                color = MaterialTheme.colorScheme.onSurfaceVariant,
                                style = MaterialTheme.typography.bodyLarge
                            )
                        },
                        singleLine = true
                    )
                    OutlinedTextField(
                        onValueChange = { taskDescriptionText = it },
                        value = taskDescriptionText,
                        label = {
                            Text(
                                text = "Description",
                                color = MaterialTheme.colorScheme.onSurfaceVariant,
                                style = MaterialTheme.typography.bodyLarge
                            )
                        },
                    )
                    Spacer(Modifier.height(8.dp))
                    Button(
                        onClick = {
                            if (taskTitleText.isBlank()) {
                                Toast.makeText(
                                    activity,
                                    "Title cannot be empty",
                                    Toast.LENGTH_SHORT
                                ).show()
                                return@Button
                            }
                            if (taskDescriptionText.isBlank()) {
                                Toast.makeText(
                                    activity,
                                    "Description cannot be empty",
                                    Toast.LENGTH_SHORT
                                ).show()
                                return@Button
                            }
                            scope.launch { sheetState.hide() }.invokeOnCompletion {
                                val newTask =
                                    Task(null, taskTitleText, taskDescriptionText)
                                viewModel.addTask(newTask)
                                isSheetOpen = false
                            }
                        },
                    ) {
                        Text("Create new")
                    }
                }
            }
        }
    }
}
