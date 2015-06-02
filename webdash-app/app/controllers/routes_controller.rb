class RoutesController < ApplicationController

  before_action :authenticate_user!

  def show
    @vehicle_route = VehicleRoute.find params[:id]
    @route = @vehicle_route.route
    @vehicle = @vehicle_route.vehicle
  end

end
