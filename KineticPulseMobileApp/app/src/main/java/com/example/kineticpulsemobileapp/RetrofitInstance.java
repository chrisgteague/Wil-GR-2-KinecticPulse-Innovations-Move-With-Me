package com.example.kineticpulsemobileapp;

import retrofit2.Retrofit;
import retrofit2.converter.gson.GsonConverterFactory;

public class RetrofitInstance {

    // Base URL for the API
    private static final String BASE_URL = "https://kinecticpulseapi.onrender.com/";

    // The Retrofit instance
    private static Retrofit retrofit;

    // Private constructor to prevent instantiation
    private RetrofitInstance() {}

    // Method to get the Retrofit instance lazily
    public static APIHandler getApi() {
        if (retrofit == null) {
            retrofit = new Retrofit.Builder()
                    .baseUrl(BASE_URL)
                    .addConverterFactory(GsonConverterFactory.create())
                    .build();
        }
        return retrofit.create(APIHandler.class);
    }
}
