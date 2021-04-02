package io.literal.ui.activity;

import android.os.Bundle;

import androidx.annotation.Nullable;
import androidx.appcompat.app.AppCompatActivity;

import io.literal.lib.Thunk;
import io.literal.repository.ErrorRepository;

public class SentryActivity extends AppCompatActivity {

    private Thunk errorRepositoryCleanup;

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        errorRepositoryCleanup = ErrorRepository.initialize();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();

        errorRepositoryCleanup.invoke();
    }
}
