class User < ActiveRecord::Base
  # Include default devise modules. Others available are:
  devise :database_authenticatable, :trackable
end
