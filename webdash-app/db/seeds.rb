# This file should contain all the record creation needed to seed the database with its default values.
# The data can then be loaded with the rake db:seed (or created alongside the db with db:setup).
#
# Examples:
#
#   cities = City.create([{ name: 'Chicago' }, { name: 'Copenhagen' }])
#   Mayor.create(name: 'Emanuel', city: cities.first)

u = User.find_by_email('nca@gmail.com')
if u.nil?
  u = User.create(email: 'nca@gmail.com', password: 'secret')
end

r = Route.find_by_name('Bangkok to Chiang Mai')
if r.nil?
  r = Route.create(name: 'Bangkok to Chiang Mai')
end

v = Vehicle.find_by_vehicle_id('NCA001')
if v.nil?
  v = Vehicle.create(vehicle_id: 'NCA001')
end

vr = VehicleRoute.find_by_vehicle_id_and_route_id(v.id, r.id)
if vr.nil?
  vr = VehicleRoute.create(vehicle_id: v.id, route_id: r.id)
end
vr.start_time = 1.minute.ago
vr.finish_time = 6.hours.from_now

if !Driver.exists? 
  d = Driver.new
  d.id = 1
  d.lane_change_frequency = 0
  d.save
end
