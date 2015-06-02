require 'test_helper'

class VehicleTest < ActiveSupport::TestCase

  test "should be valid" do
    Vehicle.all.each do |r|
      assert r.valid?
    end
  end

  test "should validate" do
    v = Vehicle.new vehicle_id: vehicles(:one).vehicle_id
    assert !v.valid?
    assert_equal ["has already been taken"], v.errors["vehicle_id"]
  end

end
