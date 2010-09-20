# This file should contain all the record creation needed to seed the database with its default values.
# The data can then be loaded with the rake db:seed (or created alongside the db with db:setup).
#
# Examples:
#
#   cities = City.create([{ :name => 'Chicago' }, { :name => 'Copenhagen' }])
#   Mayor.create(:name => 'Daley', :city => cities.first)

weight_exercise_type = { :name => "weights", :v1 => "weight", :v2 => "reps", :v3 => "rest" }
et = ExerciseType.find :first, :conditions => weight_exercise_type
ExerciseType.create!(weight_exercise_type) unless et

cardio_exercise_type = { :name => "cardio", :v1 => "workout_id", :v2 => "time", :v3 => "difficulty" }
et = ExerciseType.find :first, :conditions => cardio_exercise_type
ExerciseType.create!(cardio_exercise_type) unless et

cardio_exercise_type = { :name => "stretch", :v1 => "hold_for", :v2 => "rest" }
et = ExerciseType.find :first, :conditions => cardio_exercise_type
ExerciseType.create!(cardio_exercise_type) unless et
