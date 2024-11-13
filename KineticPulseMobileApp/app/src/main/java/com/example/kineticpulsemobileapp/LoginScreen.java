package com.example.kineticpulsemobileapp;

import android.content.Intent;
import android.os.Bundle;
import android.widget.Button;
import android.widget.EditText;
import android.widget.Toast;

import androidx.appcompat.app.AppCompatActivity;

import com.google.firebase.auth.FirebaseAuth;

public class LoginScreen extends AppCompatActivity {
    private Button regPageBtn;
    private EditText emailEditText;
    private EditText passwordEditText;
    private Button loginButton;
    private FirebaseAuth auth;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_login_screen);

        auth = FirebaseAuth.getInstance();

        // Registration page clicked
        regPageBtn = findViewById(R.id.btnRegPage);
        regPageBtn.setOnClickListener(v -> {
            Intent regIntent = new Intent(LoginScreen.this, RegisterScreen.class);
            startActivity(regIntent);
        });

        emailEditText = findViewById(R.id.logEmail);
        passwordEditText = findViewById(R.id.logPassword);
        loginButton = findViewById(R.id.btnLogin);

        loginButton.setOnClickListener(v -> {
            String email = emailEditText.getText().toString().trim();
            String password = passwordEditText.getText().toString().trim();

            if (email.isEmpty() || password.isEmpty()) {
                Toast.makeText(LoginScreen.this, "Please fill in all fields", Toast.LENGTH_SHORT).show();
            } else {
                auth.signInWithEmailAndPassword(email, password)
                        .addOnCompleteListener(this, task -> {
                            if (task.isSuccessful()) {
                                Toast.makeText(LoginScreen.this, "Login successful", Toast.LENGTH_SHORT).show();
                                startActivity(new Intent(LoginScreen.this, MainActivity.class));
                                finish();
                            } else {
                                // Login failed
                                Toast.makeText(LoginScreen.this, "Login failed. Please try again.", Toast.LENGTH_SHORT).show();
                            }
                        });
            }
        });
    }
}