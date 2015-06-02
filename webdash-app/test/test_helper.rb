ENV['RAILS_ENV'] ||= 'test'
require File.expand_path('../../config/environment', __FILE__)
require 'rails/test_help'

module TestHelper
  def load_seed_data
    load File.dirname(__FILE__) + '/../db/seeds.rb'
  end
end


class ActiveSupport::TestCase
  # Setup all fixtures in test/fixtures/*.yml for all tests in alphabetical order.
  fixtures :all
  include TestHelper
  # Add more helper methods to be used by all tests here...
end
