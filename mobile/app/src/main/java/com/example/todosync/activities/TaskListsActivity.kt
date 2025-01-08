package com.example.todosync.activities

import android.os.Bundle
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import androidx.activity.enableEdgeToEdge
import androidx.compose.foundation.BorderStroke
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.PaddingValues
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.size
import androidx.compose.foundation.lazy.LazyColumn
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.automirrored.filled.List
import androidx.compose.material.icons.filled.Add
import androidx.compose.material.icons.filled.Search
import androidx.compose.material3.BottomAppBar
import androidx.compose.material3.Button
import androidx.compose.material3.Card
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
import androidx.compose.material3.rememberModalBottomSheetState
import androidx.compose.runtime.Composable
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.saveable.rememberSaveable
import androidx.compose.runtime.setValue
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.text.style.TextOverflow
import androidx.compose.ui.tooling.preview.Preview
import androidx.compose.ui.unit.dp
import com.example.todosync.activities.ui.theme.ToDoSyncTheme

@OptIn(ExperimentalMaterial3Api::class)
class TaskListsActivity : ComponentActivity() {

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        enableEdgeToEdge()
        setContent {
            ToDoSyncTheme {
                val sheetState = rememberModalBottomSheetState()
                var isSheetOpen by rememberSaveable { mutableStateOf(false) }
                var buttonText by rememberSaveable { mutableStateOf("") }

                Scaffold(
                    modifier = Modifier
                        .fillMaxSize()
                        .padding(vertical = 5.dp),
                    containerColor = MaterialTheme.colorScheme.background,
                    bottomBar = {
                        BottomAppBar(
                            actions = {
                                IconButton(onClick = {
                                    isSheetOpen = true
                                    buttonText = "JOIN"
                                }) {
                                    Icon(
                                        Icons.Filled.Search,
                                        contentDescription = "Join a task list",
                                    )
                                }
                            },
                            floatingActionButton = {
                                FloatingActionButton(
                                    onClick = {
                                        isSheetOpen = true
                                        buttonText = "CREATE"
                                    },
                                    containerColor = MaterialTheme.colorScheme.primaryContainer,
                                    elevation = FloatingActionButtonDefaults.bottomAppBarFabElevation()
                                ) {
                                    Icon(
                                        Icons.Filled.Add,
                                        contentDescription = "Add a new task list",
                                        tint = MaterialTheme.colorScheme.onPrimaryContainer
                                    )
                                }
                            },
                            containerColor = MaterialTheme.colorScheme.surfaceContainer,
                            modifier = Modifier.fillMaxWidth()
                        )
                    }
                ) { innerPadding ->
                    LazyColumn(
                        verticalArrangement = Arrangement.spacedBy(8.dp),
                        modifier = Modifier
                            .padding(innerPadding)
                    ) {
                        items(20) {
                            TaskListCard("Task list ${it + 1}")
                        }
                    }

                    if (isSheetOpen) {
                        ModalBottomSheet(
                            sheetState = sheetState,
                            onDismissRequest = { isSheetOpen = false },
                            containerColor = MaterialTheme.colorScheme.surfaceContainerLow
                        ) {
                            TaskListBottomSheetModalContent(buttonText)
                        }
                    }

                }
            }
        }
    }

}

@Composable
fun TaskListCard(title: String, modifier: Modifier = Modifier) {
    OutlinedCard(
        border = BorderStroke(1.dp, MaterialTheme.colorScheme.outlineVariant),
        colors = CardDefaults.cardColors(containerColor = MaterialTheme.colorScheme.surface),
        modifier = modifier
            .fillMaxWidth()
            .padding(horizontal = 10.dp)
    ) {
        Row {
            Card(
                colors = CardDefaults.cardColors(
                    containerColor = MaterialTheme.colorScheme.primary
                ),
                modifier = Modifier
                    .size(60.dp)
            ) {
                Column(
                    horizontalAlignment = Alignment.CenterHorizontally,
                    verticalArrangement = Arrangement.Center,
                    modifier = Modifier.fillMaxSize()
                ) {
                    Icon(
                        Icons.AutoMirrored.Filled.List,
                        contentDescription = null,
                        tint = MaterialTheme.colorScheme.onPrimary,
                        modifier = Modifier.size(18.dp)
                    )
                }
            }
            Box(
                modifier = Modifier
                    .fillMaxWidth()
                    .padding(PaddingValues(horizontal = 16.dp))
                    .align(Alignment.CenterVertically)
            ) {
                Text(
                    text = title,
                    style = MaterialTheme.typography.titleMedium,
                    maxLines = 1,
                    overflow = TextOverflow.Ellipsis
                )
            }
        }
    }
}

@Composable
fun TaskListBottomSheetModalContent(buttonText: String) {
    Column(
        horizontalAlignment = Alignment.CenterHorizontally,
        verticalArrangement = Arrangement.Center,
        modifier = Modifier.fillMaxWidth(),
    ) {
        OutlinedTextField(
            onValueChange = {},
            value = "",
            label = { Text("Name") })
        Spacer(Modifier.height(8.dp))
        Button(onClick = {}) {
            Text(text = buttonText)
        }
    }
}

@Preview(showBackground = true)
@Composable
fun GreetingPreview2() {
    ToDoSyncTheme {
        TaskListCard("Task list 1")
    }
}