class Route < ActiveRecord::Base
  has_many :vehicle_routes
  validates_uniqueness_of :name
end
