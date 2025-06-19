package io.github.reminecraftpe;

import android.os.Build;
import android.os.Bundle;
import android.view.WindowManager;

import org.libsdl.app.LimitedSDLActivity;

public class MainActivity extends LimitedSDLActivity {
    private final ImmersiveModeStrategy immersiveModeStrategy = ImmersiveModeStrategy.create(this);

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        // Fullscreen
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.P) {
            getWindow().getAttributes().layoutInDisplayCutoutMode = WindowManager.LayoutParams.LAYOUT_IN_DISPLAY_CUTOUT_MODE_SHORT_EDGES;
        }
        immersiveModeStrategy.onCreate();
    }

    @Override
    public void onWindowFocusChanged(boolean hasFocus) {
        super.onWindowFocusChanged(hasFocus);

        // Fullscreena
        immersiveModeStrategy.onWindowFocusChanged(hasFocus);
		if (hasFocus) {
			hideSystemUI();
		}
    }
	
	private void hideSystemUI() {
		getWindow().getDecorView().setSystemUiVisibility(
            View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY
            | View.SYSTEM_UI_FLAG_HIDE_NAVIGATION
            | View.SYSTEM_UI_FLAG_FULLSCREEN
            | View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION
            | View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN
            | View.SYSTEM_UI_FLAG_LAYOUT_STABLE
		);
}
