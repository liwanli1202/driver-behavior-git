class AddMostRecentLaneChangeSpeedToDriver < ActiveRecord::Migration
  def change
    add_column :drivers, :most_recent_lane_change_speed, :integer
  end
end
