package com.example.kineticpulsemobileapp;

import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.widget.Button;
import android.widget.EditText;
import android.widget.Toast;

import androidx.appcompat.app.AppCompatActivity;
import androidx.biometric.BiometricManager;
import androidx.biometric.BiometricPrompt;
import androidx.core.content.ContextCompat;

import com.google.android.gms.auth.api.signin.GoogleSignIn;
import com.google.android.gms.auth.api.signin.GoogleSignInAccount;
import com.google.android.gms.auth.api.signin.GoogleSignInClient;
import com.google.android.gms.auth.api.signin.GoogleSignInOptions;
import com.google.android.gms.common.api.ApiException;
import com.google.android.gms.tasks.Task;
import com.google.firebase.auth.FirebaseAuth;
import com.google.firebase.auth.GoogleAuthProvider;

import org.json.JSONArray;
import org.json.JSONObject;

import java.io.BufferedReader;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.util.concurrent.Executor;

import retrofit2.Call;
import retrofit2.Callback;
import retrofit2.Response;

public class LoginScreen extends AppCompatActivity {
    private static final int RC_SIGN_IN = 9001;

    private Button regPageBtn;
    private Button loginButton;
    private Button btnGoogleSignIn;
    private EditText emailEditText;
    private EditText passwordEditText;

    private FirebaseAuth auth;
    private GoogleSignInClient googleSignInClient;

    private BiometricPrompt biometricPrompt;
    private BiometricPrompt.PromptInfo promptInfo;
    private Executor executor;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_login_screen);

        auth = FirebaseAuth.getInstance();

        // Google Sign-In configuration
        GoogleSignInOptions gso = new GoogleSignInOptions.Builder(GoogleSignInOptions.DEFAULT_SIGN_IN)
                .requestIdToken(getWebClientId()) // Dynamically retrieve the web client ID
                .requestEmail()
                .build();


        googleSignInClient = GoogleSignIn.getClient(this, gso);

        // Initialize views
        regPageBtn = findViewById(R.id.btnRegPage);
        loginButton = findViewById(R.id.btnLogin);
        btnGoogleSignIn = findViewById(R.id.btnGoogleSignIn);
        emailEditText = findViewById(R.id.logEmail);
        passwordEditText = findViewById(R.id.logPassword);

        // Biometric Authentication setup
        executor = ContextCompat.getMainExecutor(this);
        biometricPrompt = new BiometricPrompt(this, executor, new BiometricPrompt.AuthenticationCallback() {
            @Override
            public void onAuthenticationSucceeded(BiometricPrompt.AuthenticationResult result) {
                super.onAuthenticationSucceeded(result);
                // If biometric authentication is successful, proceed with Google Sign-In
                signInWithGoogle();
            }

            @Override
            public void onAuthenticationFailed() {
                super.onAuthenticationFailed();
                Toast.makeText(LoginScreen.this, "Biometric authentication failed", Toast.LENGTH_SHORT).show();
            }

            @Override
            public void onAuthenticationError(int errorCode, CharSequence errString) {
                super.onAuthenticationError(errorCode, errString);
                Toast.makeText(LoginScreen.this, "Biometric error: " + errString, Toast.LENGTH_SHORT).show();
            }
        });

        promptInfo = new BiometricPrompt.PromptInfo.Builder()
                .setTitle("Biometric Authentication")
                .setSubtitle("Authenticate using your fingerprint or face")
                .setNegativeButtonText("Cancel")
                .build();

        // Check for biometric capability
        BiometricManager biometricManager = BiometricManager.from(this);
        btnGoogleSignIn.setOnClickListener(v -> {
            if (biometricManager.canAuthenticate(BiometricManager.Authenticators.BIOMETRIC_STRONG
                    | BiometricManager.Authenticators.DEVICE_CREDENTIAL) == BiometricManager.BIOMETRIC_SUCCESS) {
                biometricPrompt.authenticate(promptInfo);
            } else {
                signInWithGoogle();
            }
        });

        // Registration page clicked
        regPageBtn.setOnClickListener(v -> {
            Intent regIntent = new Intent(LoginScreen.this, RegisterScreen.class);
            startActivity(regIntent);
        });

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
                                Toast.makeText(LoginScreen.this, "Login failed. Please try again.", Toast.LENGTH_SHORT).show();
                            }
                        });
            }
        });
    }

    private void signInWithGoogle() {
        Intent signInIntent = googleSignInClient.getSignInIntent();
        startActivityForResult(signInIntent, RC_SIGN_IN);
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);

        if (requestCode == RC_SIGN_IN) {
            Task<GoogleSignInAccount> task = GoogleSignIn.getSignedInAccountFromIntent(data);
            handleSignInResult(task);
        }
    }

    private void handleSignInResult(Task<GoogleSignInAccount> completedTask) {
        try {
            GoogleSignInAccount account = completedTask.getResult(ApiException.class);
            firebaseAuthWithGoogle(account);
        } catch (ApiException e) {
            Log.w("LoginScreen", "Google sign-in failed: " + e.getStatusCode());
            Toast.makeText(this, "Google Sign-In failed: " + e.getStatusCode(), Toast.LENGTH_SHORT).show();
        }
    }

    private void firebaseAuthWithGoogle(GoogleSignInAccount account) {
        auth.signInWithCredential(GoogleAuthProvider.getCredential(account.getIdToken(), null))
                .addOnCompleteListener(this, task -> {
                    if (task.isSuccessful()) {
                        saveUsernameToDatabase(account.getDisplayName(), account);
                    } else {
                        Toast.makeText(LoginScreen.this, "Authentication Failed", Toast.LENGTH_SHORT).show();
                    }
                });
    }

    private void saveUsernameToDatabase(String username, GoogleSignInAccount account) {
        String currentUserUid = auth.getCurrentUser().getUid();

        RegisterRequest registerRequest = new RegisterRequest(username, currentUserUid);
        RetrofitInstance.getApi().registerUser(registerRequest).enqueue(new Callback<ApiResponse>() {
            @Override
            public void onResponse(Call<ApiResponse> call, Response<ApiResponse> response) {
                if (response.isSuccessful()) {
                    startActivity(new Intent(LoginScreen.this, MainActivity.class));
                    finish();
                } else {
                    Toast.makeText(LoginScreen.this, "Failed to save username to API", Toast.LENGTH_SHORT).show();
                }
            }

            @Override
            public void onFailure(Call<ApiResponse> call, Throwable t) {
                Toast.makeText(LoginScreen.this, "Error: " + t.getMessage(), Toast.LENGTH_SHORT).show();
            }
        });
    }

    private String getWebClientId() {
        try {
            // Get the JSON resource
            InputStream inputStream = getResources().openRawResource(R.raw.google_services);
            BufferedReader reader = new BufferedReader(new InputStreamReader(inputStream));
            StringBuilder jsonStringBuilder = new StringBuilder();
            String line;
            while ((line = reader.readLine()) != null) {
                jsonStringBuilder.append(line);
            }

            // Parse the JSON to extract the web client ID
            JSONObject jsonObject = new JSONObject(jsonStringBuilder.toString());
            JSONArray clients = jsonObject.getJSONArray("client");
            for (int i = 0; i < clients.length(); i++) {
                JSONObject client = clients.getJSONObject(i);
                if (client.has("oauth_client")) {
                    JSONArray oauthClients = client.getJSONArray("oauth_client");
                    for (int j = 0; j < oauthClients.length(); j++) {
                        JSONObject oauthClient = oauthClients.getJSONObject(j);
                        if (oauthClient.getString("client_type").equals("3")) { // Type 3 is Web Client
                            return oauthClient.getString("client_id");
                        }
                    }
                }
            }
        } catch (Exception e) {
            e.printStackTrace();
        }

        throw new IllegalStateException("Web Client ID not found in google-services.json");
    }

}
