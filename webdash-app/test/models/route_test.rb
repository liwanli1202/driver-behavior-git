require 'test_helper'

class RouteTest < ActiveSupport::TestCase

  test "should be valid" do
    Route.all.each do |r|
      assert r.valid?
    end
  end

  test "should validate" do
    r = Route.new name: routes(:one).name
    assert !r.valid?
    assert_equal ["has already been taken"], r.errors["name"]
  end

end
