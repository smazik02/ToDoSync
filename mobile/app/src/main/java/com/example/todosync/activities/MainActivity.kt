package com.example.todosync.activities

import android.content.Intent
import android.os.Bundle
import android.widget.Toast
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
import com.example.todosync.ToDoSyncApplication
import com.example.todosync.network.TcpClientSingleton
import com.example.todosync.ui.theme.ToDoSyncTheme
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.launch
import kotlinx.coroutines.withContext
import java.io.IOException

class MainActivity : ComponentActivity() {

    private val tcpRepository by lazy { (application as ToDoSyncApplication).tcpRepository }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        enableEdgeToEdge()
        TcpClientSingleton.reset()
        setContent {
            val scope = rememberCoroutineScope()

            var addressText by remember { mutableStateOf("") }
            var userNameText by remember { mutableStateOf("") }

            ToDoSyncTheme {
                Scaffold(modifier = Modifier.fillMaxSize()) { innerPadding ->
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
                                            Toast.makeText(
                                                this@MainActivity,
                                                "Username cannot be blank",
                                                Toast.LENGTH_SHORT
                                            ).show()
                                            return@Button
                                        }

                                        val context = this@MainActivity

                                        scope.launch {
                                            try {
                                                withContext(Dispatchers.IO) {
                                                    tcpRepository.login(
                                                        addressText.trim(),
                                                        userNameText.trim()
                                                    )
                                                }

                                                withContext(Dispatchers.Main) {
                                                    val intent = Intent(
                                                        context,
                                                        TaskListsActivity::class.java
                                                    )
                                                    context.startActivity(intent)
                                                }
                                            } catch (e: IOException) {
                                                Toast.makeText(
                                                    this@MainActivity,
                                                    e.message,
                                                    Toast.LENGTH_SHORT
                                                ).show()
                                            }
                                        }
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