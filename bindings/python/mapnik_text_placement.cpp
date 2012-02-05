/*****************************************************************************
 *
 * This file is part of Mapnik (c++ mapping toolkit)
 *
 * Copyright (C) 2006 Artem Pavlenko, Jean-Francois Doyon
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 *****************************************************************************/
//$Id$

#include <boost/python.hpp>

#include <mapnik/text_placements.hpp>
#include "mapnik_enumeration.hpp"
#include <mapnik/expression_string.hpp>

using namespace mapnik;
//using mapnik::color;
//using mapnik::text_symbolizer;
//using mapnik::expr_node;
//using mapnik::expression_ptr;
//using mapnik::to_expression_string;

namespace {
using namespace boost::python;

tuple get_displacement(text_symbolizer_properties const& t)
{
    return boost::python::make_tuple(t.displacement.first, t.displacement.second);
}

void set_displacement(text_symbolizer_properties &t, boost::python::tuple arg)
{
    if (len(arg) != 2)
    {
        PyErr_SetObject(PyExc_ValueError,
                        ("expected 2-item tuple in call to set_displacement; got %s"
                         % arg).ptr()
            );
        throw_error_already_set();
    }

    double x = extract<double>(arg[0]);
    double y = extract<double>(arg[1]);
    t.displacement = std::make_pair(x, y);
}

/* boost.python documentation doesn't relly tell you how to do it.
But this helps:
http://www.gamedev.net/topic/446225-inheritance-in-boostpython/
*/

struct NodeWrap: formating::node, wrapper<formating::node>
{
    NodeWrap() : formating::node(), wrapper<formating::node>()
    {

    }

    void apply(char_properties const& p, Feature const& feature, processed_text &output) const
    {
        override o = this->get_override("apply");
    #ifdef MAPNIK_DEBUG
        if (!o) {
            std::cerr << "WARNING: No apply function found in FormatingNode!\n";
        }
    #endif
        o(ptr(&p), ptr(&feature), ptr(&output));
    }

    virtual void add_expressions(expression_set &output) const
    {
        override o = this->get_override("add_expressions");
        if (o)
        {
            o(ptr(&output));
        } else
        {
            formating::node::add_expressions(output);
        }
    }

    void default_add_expressions(expression_set &output) const
    {
        formating::node::add_expressions(output);
    }
};


struct TextNodeWrap: formating::text_node, wrapper<formating::text_node>
{

    TextNodeWrap(expression_ptr text) : formating::text_node(text), wrapper<formating::text_node>()
    {

    }

    virtual void apply(char_properties const& p, Feature const& feature, processed_text &output) const
    {
        if(override func_apply = this->get_override("apply"))
        {
            func_apply(ptr(&p), ptr(&feature), ptr(&output));
        }
        else
        {
            formating::text_node::apply(p, feature, output);
        }
    }

    void default_apply(char_properties const& p, Feature const& feature, processed_text &output) const
    {
        formating::text_node::apply(p, feature, output);
    }

};

struct TextPlacementsWrap: text_placements, wrapper<text_placements>
{
    text_placement_info_ptr get_placement_info(double scale_factor_, dimension_type dim,
                            bool has_dimensions_) const
    {
        return this->get_override("get_placement_info")();
    }
};

struct TextPlacementInfoWrap: text_placement_info, wrapper<text_placement_info>
{
    TextPlacementInfoWrap(text_placements const* parent,
                        double scale_factor_, dimension_type dim, bool has_dimensions_)
        : text_placement_info(parent, scale_factor_, dim, has_dimensions_)
    {

    }

    bool next()
    {
        return this->get_override("next")();
    }
};

void insert_expression(expression_set *set, expression_ptr p)
{
    set->insert(p);
}

}

void export_text_placement()
{
    using namespace boost::python;

    enumeration_<label_placement_e>("label_placement")
        .value("LINE_PLACEMENT",LINE_PLACEMENT)
        .value("POINT_PLACEMENT",POINT_PLACEMENT)
        .value("VERTEX_PLACEMENT",VERTEX_PLACEMENT)
        .value("INTERIOR_PLACEMENT",INTERIOR_PLACEMENT)
        ;
    enumeration_<vertical_alignment_e>("vertical_alignment")
        .value("TOP",V_TOP)
        .value("MIDDLE",V_MIDDLE)
        .value("BOTTOM",V_BOTTOM)
        .value("AUTO",V_AUTO)
        ;

    enumeration_<horizontal_alignment_e>("horizontal_alignment")
        .value("LEFT",H_LEFT)
        .value("MIDDLE",H_MIDDLE)
        .value("RIGHT",H_RIGHT)
        .value("AUTO",H_AUTO)
        ;

    enumeration_<justify_alignment_e>("justify_alignment")
        .value("LEFT",J_LEFT)
        .value("MIDDLE",J_MIDDLE)
        .value("RIGHT",J_RIGHT)
        ;

    enumeration_<text_transform_e>("text_transform")
        .value("NONE",NONE)
        .value("UPPERCASE",UPPERCASE)
        .value("LOWERCASE",LOWERCASE)
        .value("CAPITALIZE",CAPITALIZE)
        ;


    class_<text_symbolizer_properties>("TextSymbolizerProperties")
        .def_readwrite("orientation", &text_symbolizer_properties::orientation)
        .add_property("displacement",
                      &get_displacement,
                      &set_displacement)
        .def_readwrite("label_placement", &text_symbolizer_properties::label_placement)
        .def_readwrite("horizontal_alignment", &text_symbolizer_properties::halign)
        .def_readwrite("justify_alignment", &text_symbolizer_properties::jalign)
        .def_readwrite("vertical_alignment", &text_symbolizer_properties::valign)
        .def_readwrite("label_spacing", &text_symbolizer_properties::label_spacing)
        .def_readwrite("label_position_tolerance", &text_symbolizer_properties::label_position_tolerance)
        .def_readwrite("avoid_edges", &text_symbolizer_properties::avoid_edges)
        .def_readwrite("minimum_distance", &text_symbolizer_properties::minimum_distance)
        .def_readwrite("minimum_padding", &text_symbolizer_properties::minimum_padding)
        .def_readwrite("minimum_path_length", &text_symbolizer_properties::minimum_path_length)
        .def_readwrite("maximum_angle_char_delta", &text_symbolizer_properties::max_char_angle_delta)
        .def_readwrite("force_odd_labels", &text_symbolizer_properties::force_odd_labels)
        .def_readwrite("allow_overlap", &text_symbolizer_properties::allow_overlap)
        .def_readwrite("text_ratio", &text_symbolizer_properties::text_ratio)
        .def_readwrite("wrap_width", &text_symbolizer_properties::wrap_width)
        .def_readwrite("default_format", &text_symbolizer_properties::default_format)
        .add_property ("format_tree",
                       &text_symbolizer_properties::format_tree,
                       &text_symbolizer_properties::set_format_tree);
        /* from_xml, to_xml operate on mapnik's internal XML tree and don't make sense in python.
        get_all_expressions isn't useful in python either. The result is only needed by
        attribute_collector (which isn't exposed in python) and
        it just calls add_expressions of the associated formating tree.
        set_old_style expression is just a compatibility wrapper and doesn't need to be exposed in python. */
        ;

    class_<char_properties>("CharProperties")
        .def_readwrite("face_name", &char_properties::face_name)
        .def_readwrite("fontset", &char_properties::fontset)
        .def_readwrite("text_size", &char_properties::text_size)
        .def_readwrite("character_spacing", &char_properties::character_spacing)
        .def_readwrite("line_spacing", &char_properties::line_spacing)
        .def_readwrite("text_opacity", &char_properties::text_opacity)
        .def_readwrite("wrap_char", &char_properties::wrap_char)
        .def_readwrite("wrap_before", &char_properties::wrap_before)
        .def_readwrite("text_transform", &char_properties::text_transform)
        .def_readwrite("fill", &char_properties::fill)
        .def_readwrite("halo_fill", &char_properties::halo_fill)
        .def_readwrite("halo_radius", &char_properties::halo_radius)
        /* from_xml, to_xml operate on mapnik's internal XML tree and don't make sense in python.*/
        ;

    class_<TextPlacementsWrap,
           boost::shared_ptr<TextPlacementsWrap>,
           boost::noncopyable>
           ("TextPlacements")
        .def_readwrite("defaults", &text_placements::properties)
        .def("get_placement_info", pure_virtual(&text_placements::get_placement_info))
        /* TODO: get_all_expressions() */
        ;
    register_ptr_to_python<boost::shared_ptr<text_placements> >();

    class_<TextPlacementInfoWrap,
           boost::shared_ptr<TextPlacementInfoWrap>,
           boost::noncopyable>
           ("TextPlacementInfo",
            init<text_placements const*, double, dimension_type, bool>())
        .def("next", pure_virtual(&text_placement_info::next))
        .def("get_actual_label_spacing", &text_placement_info::get_actual_label_spacing)
        .def("get_actual_minimum_distance", &text_placement_info::get_actual_minimum_distance)
        .def("get_actual_minimum_padding", &text_placement_info::get_actual_minimum_padding)
        .def_readwrite("properties", &text_placement_info::properties)
        .def_readwrite("scale_factor", &text_placement_info::scale_factor)
        .def_readwrite("has_dimensions", &text_placement_info::has_dimensions)
        .def_readwrite("dimensions", &text_placement_info::dimensions)
        .def_readwrite("collect_extents", &text_placement_info::collect_extents)
        .def_readwrite("extents", &text_placement_info::extents)
        .def_readwrite("additional_boxes", &text_placement_info::additional_boxes)
        .def_readwrite("envelopes", &text_placement_info::envelopes)
//        .def_readwrite("placements", &text_placement_info::placements)
        ;
    register_ptr_to_python<boost::shared_ptr<text_placement_info> >();

    class_<processed_text,
            boost::shared_ptr<processed_text>,
            boost::noncopyable>("ProcessedText", no_init)
        .def("push_back", &processed_text::push_back)
        .def("clear", &processed_text::clear)
        ;

    class_<expression_set,
            boost::shared_ptr<expression_set>,
            boost::noncopyable>("ExpressionSet")
            ;
    def("insert_expression", insert_expression);

    register_ptr_to_python<boost::shared_ptr<expression_set> >();


    //TODO: Python namespace
    class_<NodeWrap,
           boost::shared_ptr<NodeWrap>,
           boost::noncopyable>
           ("FormatingNode")
        .def("apply", pure_virtual(&formating::node::apply))
        .def("add_expressions",
             &NodeWrap::add_expressions,
             &NodeWrap::default_add_expressions)
        ;

    register_ptr_to_python<boost::shared_ptr<formating::node> >();

    class_<TextNodeWrap,
           boost::shared_ptr<TextNodeWrap>,
           bases<formating::node>,
           boost::noncopyable>(
                "FormatingTextNode", init<expression_ptr>())
            .def("apply", &formating::text_node::apply, &TextNodeWrap::default_apply)
            .add_property("text",
                          &formating::text_node::get_text,
                          &formating::text_node::set_text)
        ;

    register_ptr_to_python<boost::shared_ptr<formating::text_node> >();

}
