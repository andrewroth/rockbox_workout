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
# It's strongly recommended to check this file into your version control system.

ActiveRecord::Schema.define(:version => 20101201053320) do

  create_table "exercise_log_entries", :force => true do |t|
    t.integer  "workout_log_entry_id"
    t.integer  "workout_exercise_id"
    t.integer  "n"
    t.integer  "position"
    t.datetime "created_at"
    t.datetime "updated_at"
    t.datetime "last_completed_at"
  end

  create_table "exercise_sets", :force => true do |t|
    t.string   "name"
    t.integer  "position"
    t.integer  "exercise_id"
    t.datetime "created_at"
    t.datetime "updated_at"
  end

  create_table "exercise_types", :force => true do |t|
    t.string   "name"
    t.string   "v1"
    t.string   "v2"
    t.string   "v3"
    t.string   "v4"
    t.string   "v5"
    t.string   "v6"
    t.string   "v7"
    t.datetime "created_at"
    t.datetime "updated_at"
  end

  create_table "exercises", :force => true do |t|
    t.string   "name"
    t.integer  "n"
    t.integer  "exercise_type_id"
    t.datetime "created_at"
    t.datetime "updated_at"
    t.text     "description"
  end

  create_table "functions", :force => true do |t|
    t.string   "type"
    t.decimal  "base"
    t.decimal  "inc"
    t.decimal  "round_to_nearest"
    t.decimal  "min_n"
    t.decimal  "max_n"
    t.decimal  "min_v"
    t.decimal  "max_v"
    t.datetime "created_at"
    t.datetime "updated_at"
    t.string   "variable"
    t.integer  "exercise_set_id"
  end

  create_table "set_log_entries", :force => true do |t|
    t.integer  "exercise_log_entry_id"
    t.integer  "position"
    t.decimal  "v1"
    t.decimal  "v2"
    t.decimal  "v3"
    t.decimal  "v4"
    t.decimal  "v5"
    t.decimal  "v6"
    t.decimal  "v7"
    t.datetime "created_at"
    t.datetime "updated_at"
    t.datetime "started_at"
    t.datetime "completed_at"
    t.datetime "done_rested_at"
    t.integer  "workout_set_id"
  end

  create_table "workout_dates", :force => true do |t|
    t.datetime "when"
    t.integer  "workout_id"
    t.datetime "created_at"
    t.datetime "updated_at"
    t.integer  "n"
  end

  create_table "workout_exercises", :force => true do |t|
    t.integer  "workout_id"
    t.integer  "exercise_id"
    t.integer  "position"
    t.datetime "created_at"
    t.datetime "updated_at"
  end

  create_table "workouts", :force => true do |t|
    t.string   "name"
    t.integer  "n"
    t.datetime "created_at"
    t.datetime "updated_at"
    t.text     "description"
  end

end
