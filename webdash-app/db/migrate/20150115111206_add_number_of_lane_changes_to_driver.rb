class AddNumberOfLaneChangesToDriver < ActiveRecord::Migration
  def change
    add_column :drivers, :number_of_lane_changes, :integer
  end
end
