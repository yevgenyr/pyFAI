# coding: utf-8
# /*##########################################################################
#
# Copyright (c) 2016 European Synchrotron Radiation Facility
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
#
# ###########################################################################*/

from __future__ import absolute_import

__authors__ = ["V. Valls"]
__license__ = "MIT"
__date__ = "14/02/2017"

from .AbstractModel import AbstractModel
from .DataModel import DataModel


class GeometryModel(AbstractModel):

    def __init__(self, parent=None):
        super(GeometryModel, self).__init__(parent)
        self.__distance = DataModel()
        self.__wavelength = DataModel()
        self.__poni1 = DataModel()
        self.__poni2 = DataModel()
        self.__rotation1 = DataModel()
        self.__rotation2 = DataModel()
        self.__rotation3 = DataModel()

        self.__distance.changed.connect(self.dataChanged)
        self.__wavelength.changed.connect(self.dataChanged)
        self.__poni1.changed.connect(self.dataChanged)
        self.__poni2.changed.connect(self.dataChanged)
        self.__rotation1.changed.connect(self.dataChanged)
        self.__rotation2.changed.connect(self.dataChanged)
        self.__rotation3.changed.connect(self.dataChanged)

    def isValid(self):
        if not self.__distance.isValid():
            return False
        if not self.__wavelength.isValid():
            return False
        if not self.__poni1.isValid():
            return False
        if not self.__poni2.isValid():
            return False
        if not self.__rotation1.isValid():
            return False
        if not self.__rotation2.isValid():
            return False
        if not self.__rotation3.isValid():
            return False

    def distance(self):
        return self.__distance

    def wavelength(self):
        return self.__wavelength

    def poni1(self):
        return self.__poni1

    def poni2(self):
        return self.__poni1

    def rotation1(self):
        return self.__rotation1

    def rotation2(self):
        return self.__rotation2

    def rotation3(self):
        return self.__rotation3
