class VehicleRoute < ActiveRecord::Base
  belongs_to :vehicle
  belongs_to :route

  validates_presence_of :vehicle
  validates_presence_of :route

  def name
    "#{route.name}, #{vehicle.vehicle_id}, #{start_time} to #{finish_time}"
  end

end
