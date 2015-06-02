class AddLaneChangeSpeedSumToDriver < ActiveRecord::Migration
  def change
    add_column :drivers, :lane_change_speed_sum, :integer
  end
end
