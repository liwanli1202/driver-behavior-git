require 'test_helper'

class RoutesControllerTest < ActionController::TestCase
  include Devise::TestHelpers

  test "should authorize get index" do
    get :show, id: vehicle_routes(:one).id
    assert_redirected_to new_user_session_path
  end

  test "should get show" do
    sign_in users(:one)
    get :show, id: vehicle_routes(:one).id
    assert_response :success
  end

end
