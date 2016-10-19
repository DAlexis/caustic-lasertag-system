package ru.caustic.lasertag.ui;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.widget.TextView;

import ru.caustic.lasertag.core.CausticController;
import ru.caustic.lasertag.core.GameStatistics;

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
    private class OnStatsUpdatedListener implements GameStatistics.StatsSyncDoneListener {
        @Override
        public void onStatsSyncDone()
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
        CausticController.getInstance().getGameStatistics().readStatsAsync(onStatsUpdatedListener);
    }

    private OnStatsUpdatedListener onStatsUpdatedListener = new OnStatsUpdatedListener();
}
