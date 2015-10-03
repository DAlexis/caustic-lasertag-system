package ru.caustic.lasertag.causticlasertagcontroller;

import android.app.Activity;
import android.content.Context;
import android.net.Uri;
import android.os.Bundle;
import android.app.Fragment;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.ListView;
import android.widget.TextView;

import java.util.ArrayList;


public class DeviceSettingsFragment extends Fragment {
    private MyCustomAdapter mAdapter;
    ListView parsList;

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        View view = inflater.inflate(R.layout.fragment_device_settings, container, false);
        parsList = (ListView) view.findViewById(R.id.paramtersList);
        // Inflate the layout for this fragment
        mAdapter = new MyCustomAdapter();
        for (int i = 0; i < 50; i++) {
            mAdapter.addItem("item " + i);
        }
        parsList.setAdapter(mAdapter);
        mAdapter.notifyDataSetChanged();
        return view;
    }

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

    }

    private class MyCustomAdapter extends BaseAdapter {

        private ArrayList<String> mData = new ArrayList();
        private LayoutInflater mInflater;

        public MyCustomAdapter() {
            mInflater = (LayoutInflater) getActivity().getSystemService(Context.LAYOUT_INFLATER_SERVICE);
        }

        public void addItem(final String item) {
            mData.add(item);
            notifyDataSetChanged();
        }

        @Override
        public int getCount() {
            return mData.size();
        }

        @Override
        public String getItem(int position) {
            return mData.get(position);
        }

        @Override
        public long getItemId(int position) {
            return position;
        }

        @Override
        public View getView(int position, View convertView, ViewGroup parent) {
            //System.out.println("getView " + position + " " + convertView);
            UintViewHolder holder = null;
            if (convertView == null) {
                convertView = mInflater.inflate(R.layout.paramteters_uint, null);
                holder = new UintViewHolder(convertView);
                convertView.setTag(holder);
            } else {
                holder = (UintViewHolder)convertView.getTag();
            }
            holder.name.setText(mData.get(position));
            return convertView;
        }

    }

    public interface ParametersView {

    }

    public static class UintViewHolder implements ParametersView {
        public TextView name;
        public TextView value;

        UintViewHolder(View convertView) {
            name = (TextView)convertView.findViewById(R.id.textViewParameterName);
            value = (TextView)convertView.findViewById(R.id.textViewParameterValue);
        }
    }


    public class DevicePropertyAdapter {
        private int id = 0;

        DevicePropertyAdapter(int id, int adapterType) {
            this.id = id;
        }


    }
}
