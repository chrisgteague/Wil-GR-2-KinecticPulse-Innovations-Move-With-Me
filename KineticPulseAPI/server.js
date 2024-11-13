require('dotenv').config();

const express = require('express');
const admin = require('firebase-admin');

// Initialize Firebase Admin using environment variables
admin.initializeApp({
    credential: admin.credential.cert({
        type: process.env.FIREBASE_TYPE,
        project_id: process.env.FIREBASE_PROJECT_ID,
        private_key_id: process.env.FIREBASE_PRIVATE_KEY_ID,
        private_key: process.env.FIREBASE_PRIVATE_KEY.replace(/\\n/g, '\n'), // Fix new line issue in private key
        client_email: process.env.FIREBASE_CLIENT_EMAIL,
        client_id: process.env.FIREBASE_CLIENT_ID,
        auth_uri: process.env.FIREBASE_AUTH_URI,
        token_uri: process.env.FIREBASE_TOKEN_URI,
        auth_provider_x509_cert_url: process.env.FIREBASE_AUTH_PROVIDER_CERT_URL,
        client_x509_cert_url: process.env.FIREBASE_CLIENT_CERT_URL
    }),
    databaseURL: process.env.FIREBASE_DATABASE_URL // Use your Firebase database URL from env variables
});

const app = express();
app.use(express.json()); 

// Endpoint to save username
app.post('/register', async (req, res) => {
    const { uid, username} = req.body;

    if (!uid) {
        return res.status(400).send({ error: 'User UID is required' });
    }

    try {
        // Save data under 'users/{uid}/username'
        await admin.database().ref(`users/${uid}/username`).set({
            username: username
        });
        res.status(200).send({ message: 'Username saved successfully to Firebase' });
        
        
    } catch (error) {
        res.status(500).send({ error: 'Failed to save username to Firebase: ' + error.message });
    }
});

app.post('/saveJumpData', async (req, res) => {
    const { uid, leftJump, rightJump, middleJump } = req.body;

    if (!uid) {
        return res.status(400).send({ error: 'User UID is required' });
    }

    try {
        // Save data under 'users/{uid}/jumpdata'
        await admin.database().ref(`users/${uid}/jumpdata`).set({
            leftJump : leftJump,
            rightJump : rightJump,
            middleJump : middleJump
        });
        res.status(200).send({ message: 'Jumpdata saved successfully to Firebase' });
    } catch (error) {
        res.status(500).send({ error: 'Failed to save jumpdata to Firebase: ' + error.message });
    }
});

// Endpoint to retrieve user-specific jumpdata from Firebase
app.get('/getJumpData/:uid', async (req, res) => {
    const { uid } = req.params;

    if (!uid) {
        return res.status(400).send({ error: 'User UID is required' });
    }

    try {
        console.log(`Attempting to retrieve jumpdata for user: ${uid}`);
        
        // Retrieve data under 'users/{uid}/jumpdata'
        const snapshot = await admin.database().ref(`users/${uid}/jumpdata`).once('value');
        const data = snapshot.val();
        
        console.log('Data retrieved:', data);
        
        if (data) {
            res.status(200).send(data);
           
        } else {
            res.status(404).send({ message: 'No jumpdata found for this user' });
        }
    } catch (error) {
        console.error('Error retrieving data:', error.message);
        res.status(500).send({ error: 'Failed to retrieve jumpdata from Firebase: ' + error.message });
    }
});

const PORT = process.env.PORT || 5000;
app.listen(PORT, () => {
    console.log(`Server running on port ${PORT}`);
});