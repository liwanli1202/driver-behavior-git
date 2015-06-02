class AddAvgLaneChangeSpeedToDriver < ActiveRecord::Migration
  def change
    add_column :drivers, :avg_lane_change_speed, :float
  end
end
