class Vehicle < ActiveRecord::Base
  has_many :vehicle_routes
  validates_uniqueness_of :vehicle_id
end
