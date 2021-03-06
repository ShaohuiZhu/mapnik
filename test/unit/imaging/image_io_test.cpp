#include "catch.hpp"

#include <iostream>
#include <mapnik/image.hpp>
#include <mapnik/image_reader.hpp>
#include <mapnik/image_util.hpp>
#include <mapnik/util/fs.hpp>
#include <vector>
#include <algorithm>
#if defined(HAVE_CAIRO)
#include <mapnik/cairo/cairo_context.hpp>
#include <mapnik/cairo/cairo_image_util.hpp>
#endif

TEST_CASE("image io") {

SECTION("readers") {

    std::string should_throw;
    boost::optional<std::string> type;
    try
    {
#if defined(HAVE_JPEG)
        should_throw = "./test-data/images/blank.jpg";
        REQUIRE( mapnik::util::exists( should_throw ) );
        type = mapnik::type_from_filename(should_throw);
        REQUIRE( type );
        try
        {
            std::unique_ptr<mapnik::image_reader> reader(mapnik::get_image_reader(should_throw,*type));
            REQUIRE( false );
        }
        catch (std::exception const&)
        {
            REQUIRE( true );
        }
#endif

    try
    {
        mapnik::image_rgba8 im(-10,-10); // should throw rather than overflow
        REQUIRE( im.width() < 10 ); // should not get here, but if we did this test should fail
    }
    catch (std::exception const& ex)
    {
        REQUIRE( true ); // should hit bad alloc here
    }

#if defined(HAVE_CAIRO)
        mapnik::cairo_surface_ptr image_surface(
            cairo_image_surface_create(CAIRO_FORMAT_ARGB32,256,257),
            mapnik::cairo_surface_closer());
        mapnik::image_rgba8 im_data(cairo_image_surface_get_width(&*image_surface), cairo_image_surface_get_height(&*image_surface));
        im_data.set(1);
        REQUIRE( (unsigned)im_data(0,0) == unsigned(1) );
        // Should set back to fully transparent
        mapnik::cairo_image_to_rgba8(im_data, image_surface);
        REQUIRE( (unsigned)im_data(0,0) == unsigned(0) );
#endif

#if defined(HAVE_PNG)
        should_throw = "./test-data/images/blank.png";
        REQUIRE( mapnik::util::exists( should_throw ) );
        type = mapnik::type_from_filename(should_throw);
        REQUIRE( type );
        try
        {
            std::unique_ptr<mapnik::image_reader> reader(mapnik::get_image_reader(should_throw,*type));
            REQUIRE( false );
        }
        catch (std::exception const&)
        {
            REQUIRE( true );
        }

        should_throw = "./test-data/images/xcode-CgBI.png";
        REQUIRE( mapnik::util::exists( should_throw ) );
        type = mapnik::type_from_filename(should_throw);
        REQUIRE( type );
        try
        {
            std::unique_ptr<mapnik::image_reader> reader(mapnik::get_image_reader(should_throw,*type));
            REQUIRE( false );
        }
        catch (std::exception const&)
        {
            REQUIRE( true );
        }
#endif

#if defined(HAVE_TIFF)
        should_throw = "./test-data/images/blank.tiff";
        REQUIRE( mapnik::util::exists( should_throw ) );
        type = mapnik::type_from_filename(should_throw);
        REQUIRE( type );
        try
        {
            std::unique_ptr<mapnik::image_reader> reader(mapnik::get_image_reader(should_throw,*type));
            REQUIRE( false );
        }
        catch (std::exception const&)
        {
            REQUIRE( true );
        }
#endif

#if defined(HAVE_WEBP)
        should_throw = "./test-data/images/blank.webp";
        REQUIRE( mapnik::util::exists( should_throw ) );
        type = mapnik::type_from_filename(should_throw);
        REQUIRE( type );
        try
        {
            std::unique_ptr<mapnik::image_reader> reader(mapnik::get_image_reader(should_throw,*type));
            REQUIRE( false );
        }
        catch (std::exception const&)
        {
            REQUIRE( true );
        }
#endif
    }
    catch (std::exception const & ex)
    {
        std::clog << ex.what() << "\n";
        REQUIRE(false);
    }

}
}