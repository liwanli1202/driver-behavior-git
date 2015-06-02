class AddLaneChangeFrequencyToDriver < ActiveRecord::Migration
  def change
    add_column :drivers, :lane_change_frequency, :float
  end
end
