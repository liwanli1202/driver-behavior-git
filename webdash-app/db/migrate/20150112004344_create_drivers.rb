class CreateDrivers < ActiveRecord::Migration
  def change
    create_table :drivers do |t|
      t.integer :lane_change_frequency

      t.timestamps
    end
  end
end
