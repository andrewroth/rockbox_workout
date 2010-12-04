class Function < ActiveRecord::Base
  include Csv
  belongs_to :exercise_set
end
