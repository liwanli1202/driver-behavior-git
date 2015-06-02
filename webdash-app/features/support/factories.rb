
FactoryGirl.define do

  factory :customer, class: :user do
    email 'john@example.com'
    password 'secret'
  end

  factory :vehicle do
    sequence(:vehicle_id) { |n| "NCA0#{n}" }
  end

  factory :route do
    name 'Bangkok to Chiang Mai'
  end

  factory :vehicle_route do
    vehicle
    route
    start_time 1.hour.from_now
    finish_time 12.hours.from_now
  end

end
