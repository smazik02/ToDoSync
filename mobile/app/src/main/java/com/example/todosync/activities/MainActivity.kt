package com.example.todosync.activities

import android.content.Intent
import android.os.Bundle
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import androidx.activity.enableEdgeToEdge
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.WindowInsets
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.safeContent
import androidx.compose.foundation.layout.windowInsetsPadding
import androidx.compose.material3.Button
import androidx.compose.material3.ButtonDefaults
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.OutlinedTextField
import androidx.compose.material3.Scaffold
import androidx.compose.material3.SnackbarHost
import androidx.compose.material3.SnackbarHostState
import androidx.compose.material3.Surface
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.rememberCoroutineScope
import androidx.compose.runtime.setValue
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.tooling.preview.Preview
import com.example.todosync.network.TcpRepository
import com.example.todosync.ui.theme.ToDoSyncTheme
import kotlinx.coroutines.CoroutineScope
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.launch
import java.io.IOException

class MainActivity : ComponentActivity() {

    private val tcpRepository: TcpRepository = TcpRepository()

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        enableEdgeToEdge()
        setContent {
            val scope = rememberCoroutineScope()
            val snackbarHostState = remember { SnackbarHostState() }

            var addressText by remember { mutableStateOf("") }
            var userNameText by remember { mutableStateOf("") }

            ToDoSyncTheme {
                Scaffold(
                    snackbarHost = { SnackbarHost(hostState = snackbarHostState) },
                    modifier = Modifier.fillMaxSize()
                ) { innerPadding ->
                    Surface(
                        modifier = Modifier.fillMaxSize(),
                        color = MaterialTheme.colorScheme.background
                    ) {
                        Box(
                            modifier = Modifier
                                .fillMaxSize()
                                .windowInsetsPadding(WindowInsets.safeContent)
                        ) {
                            Column(
                                modifier = Modifier
                                    .align(Alignment.Center)
                                    .fillMaxWidth()
                                    .padding(innerPadding),
                                horizontalAlignment = Alignment.End
                            ) {
                                OutlinedTextField(
                                    value = addressText,
                                    onValueChange = { addressText = it },
                                    modifier = Modifier
                                        .fillMaxWidth(),
                                    textStyle = MaterialTheme.typography.bodyLarge,
                                    label = {
                                        Text(
                                            text = "Address with port (address:port)",
                                            color = MaterialTheme.colorScheme.onSurface,
                                            style = MaterialTheme.typography.bodyLarge
                                        )
                                    }
                                )
                                OutlinedTextField(
                                    value = userNameText,
                                    onValueChange = { userNameText = it },
                                    modifier = Modifier
                                        .fillMaxWidth(),
                                    textStyle = MaterialTheme.typography.bodyLarge,
                                    label = {
                                        Text(
                                            text = "Username",
                                            color = MaterialTheme.colorScheme.onSurface,
                                            style = MaterialTheme.typography.bodyLarge
                                        )
                                    }
                                )
                                Button(
                                    onClick = {
                                        if (userNameText.isBlank()) {
                                            scope.launch {
                                                snackbarHostState.showSnackbar("Username cannot be blank")
                                            }
                                            return@Button
                                        }

                                        CoroutineScope(Dispatchers.IO).launch {
                                            try {
                                                tcpRepository.login(addressText, userNameText)
                                            } catch(e: IOException) {
                                                snackbarHostState.showSnackbar(e.message.toString())
                                            }
                                        }
//                                        Intent(
//                                            applicationContext,
//                                            TaskListsActivity::class.java
//                                        ).also {
//                                            startActivity(it)
//                                        }
                                    },
                                    colors = ButtonDefaults.buttonColors(),
                                ) {
                                    Text(
                                        text = "Login",
                                        color = MaterialTheme.colorScheme.onPrimary,
                                        style = MaterialTheme.typography.labelLarge,
                                        maxLines = 1
                                    )
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    override fun onDestroy() {
        super.onDestroy()
        tcpRepository.disconnect()
    }
}

@Composable
fun Greeting(name: String, modifier: Modifier = Modifier) {
    Text(
        text = "Hello $name!",
        modifier = modifier
    )
}

@Preview(showBackground = true)
@Composable
fun GreetingPreview() {
    ToDoSyncTheme {
        Greeting("Android")
    }
}