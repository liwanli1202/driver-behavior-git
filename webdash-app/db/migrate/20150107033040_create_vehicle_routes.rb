class CreateVehicleRoutes < ActiveRecord::Migration
  def change
    create_table :vehicle_routes do |t|
      t.references :vehicle, index: true
      t.references :route, index: true
      t.datetime :start_time
      t.datetime :finish_time

      t.timestamps
    end
  end
end
