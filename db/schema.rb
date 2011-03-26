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

ActiveRecord::Schema.define(:version => 20110326025000) do

  create_table "exercise_log_entries", :force => true do |t|
    t.integer  "workout_date_id"
    t.integer  "exercise_id"
    t.integer  "n"
    t.integer  "position"
    t.datetime "created_at"
    t.datetime "updated_at"
    t.datetime "started_at"
    t.datetime "finished_at"
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
    t.decimal  "base",             :precision => 10, :scale => 0
    t.decimal  "inc",              :precision => 10, :scale => 0
    t.decimal  "round_to_nearest", :precision => 10, :scale => 0
    t.decimal  "min_n",            :precision => 10, :scale => 0
    t.decimal  "max_n",            :precision => 10, :scale => 0
    t.decimal  "min_v",            :precision => 10, :scale => 0
    t.decimal  "max_v",            :precision => 10, :scale => 0
    t.datetime "created_at"
    t.datetime "updated_at"
    t.string   "variable"
    t.integer  "exercise_set_id"
    t.integer  "workout_id"
  end

  create_table "set_functions", :force => true do |t|
    t.integer  "set_id"
    t.string   "variable"
    t.integer  "variable_function_id"
    t.datetime "created_at"
    t.datetime "updated_at"
  end

  create_table "set_log_entries", :force => true do |t|
    t.integer  "exercise_log_entry_id"
    t.integer  "workout_set_id"
    t.integer  "position"
    t.decimal  "v1",                    :precision => 10, :scale => 0
    t.decimal  "v2",                    :precision => 10, :scale => 0
    t.decimal  "v3",                    :precision => 10, :scale => 0
    t.decimal  "v4",                    :precision => 10, :scale => 0
    t.decimal  "v5",                    :precision => 10, :scale => 0
    t.decimal  "v6",                    :precision => 10, :scale => 0
    t.decimal  "v7",                    :precision => 10, :scale => 0
    t.datetime "created_at"
    t.datetime "updated_at"
    t.datetime "started_at"
    t.datetime "completed_at"
    t.datetime "done_rested_at"
  end

  create_table "workout_dates", :force => true do |t|
    t.datetime "when"
    t.integer  "workout_id"
    t.datetime "created_at"
    t.datetime "updated_at"
    t.integer  "n"
    t.datetime "started_at"
    t.datetime "finished_at"
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
