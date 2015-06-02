require 'test_helper'

class VehicleRouteTest < ActiveSupport::TestCase

  test "should be valid" do
    VehicleRoute.all.each do |r|
      assert r.valid?, r.errors.full_messages
    end
  end

end
