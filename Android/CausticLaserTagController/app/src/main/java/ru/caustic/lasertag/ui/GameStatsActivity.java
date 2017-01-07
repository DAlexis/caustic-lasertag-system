package ru.caustic.lasertag.ui;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.widget.TextView;

import ru.caustic.rcspcore.CausticController;
import ru.caustic.rcspcore.GameStatistics;

public class GameStatsActivity extends AppCompatActivity {

    TextView textViewStatistics;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_game_stats);
        textViewStatistics = (TextView) findViewById(R.id.textViewStatistics);
        doUpdate();
    }


    // Private
    private class OnStatsUpdatedListener implements GameStatistics.StatsChangeListener {

        @Override
        public void onStatsChange(int victimId, int damagerId)
        {
            runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    textViewStatistics.setText(
                            CausticController.getInstance().getGameStatistics().getStatisticsTableStrStub(GameStatistics.DAMAGE)
                    );
                }
            });
        }
    }

    private void doUpdate() {
        textViewStatistics.setText("Updating in progress...");
        CausticController.getInstance().getGameStatistics().setStatsChangeListener(onStatsUpdatedListener);
        CausticController.getInstance().getGameStatistics().updateStats();
    }

    private OnStatsUpdatedListener onStatsUpdatedListener = new OnStatsUpdatedListener();
}
