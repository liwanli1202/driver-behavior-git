# encoding: UTF-8
# This file is auto-generated from the current state of the database. Instead
# of editing this file, please use the migrations feature of Active Record to
# incrementally modify your database, and then regenerate this schema definition.
#
# Note that this schema.rb definition is the authoritative source for your
# database schema. If you need to create the application database on another
# system, you should be using db:schema:load, not running all the migrations
# from scratch. The latter is a flawed and unsustainable approach (the more migrations
# you'll amass, the slower it'll run and the greater likelihood for issues).
#
# It's strongly recommended that you check this file into your version control system.

ActiveRecord::Schema.define(version: 20150121041722) do

  # These are extensions that must be enabled in order to support this database
  enable_extension "plpgsql"

  create_table "drivers", force: true do |t|
    t.datetime "created_at"
    t.datetime "updated_at"
    t.integer  "lane_change_speed_sum"
    t.float    "avg_lane_change_speed"
    t.integer  "most_recent_lane_change_speed"
    t.integer  "time_driven"
    t.integer  "number_of_lane_changes"
    t.float    "stability"
    t.float    "lane_change_frequency"
    t.float    "avg_stability"
    t.float    "sum_of_stability"
    t.integer  "number_of_frames"
    t.text     "lat_dis_string"
  end

  create_table "routes", force: true do |t|
    t.string   "name"
    t.datetime "created_at"
    t.datetime "updated_at"
  end

  create_table "users", force: true do |t|
    t.string   "email",              default: "", null: false
    t.string   "encrypted_password", default: "", null: false
    t.integer  "sign_in_count",      default: 0,  null: false
    t.datetime "current_sign_in_at"
    t.datetime "last_sign_in_at"
    t.inet     "current_sign_in_ip"
    t.inet     "last_sign_in_ip"
    t.datetime "created_at"
    t.datetime "updated_at"
  end

  add_index "users", ["email"], name: "index_users_on_email", unique: true, using: :btree

  create_table "vehicle_routes", force: true do |t|
    t.integer  "vehicle_id"
    t.integer  "route_id"
    t.datetime "start_time"
    t.datetime "finish_time"
    t.datetime "created_at"
    t.datetime "updated_at"
  end

  add_index "vehicle_routes", ["route_id"], name: "index_vehicle_routes_on_route_id", using: :btree
  add_index "vehicle_routes", ["vehicle_id"], name: "index_vehicle_routes_on_vehicle_id", using: :btree

  create_table "vehicles", force: true do |t|
    t.string   "vehicle_id"
    t.datetime "created_at"
    t.datetime "updated_at"
  end

end
